#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "console.hpp"
#include "fastfiles.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

namespace weapon
{
	namespace
	{
		utils::hook::detour g_setup_level_weapon_def_hook;
		void g_setup_level_weapon_def_stub()
		{
			// precache level weapons first
			g_setup_level_weapon_def_hook.invoke<void>();

			std::vector<game::WeaponDef*> weapons;

			// find all weapons in asset pools
			fastfiles::enum_assets(game::ASSET_TYPE_WEAPON, [&weapons](game::XAssetHeader header)
			{
				weapons.push_back(header.weapon);
			}, false);

			// sort weapons
			std::sort(weapons.begin(), weapons.end(), [](game::WeaponDef* weapon1, game::WeaponDef* weapon2)
			{
				return std::string_view(weapon1->name) <
					std::string_view(weapon2->name);
			});

			// precache items
			for (std::size_t i = 0; i < weapons.size(); i++)
			{
				console::debug("precaching weapon \"%s\"\n", weapons[i]->name);
				game::G_GetWeaponForName(weapons[i]->name);
			}
		}

		utils::hook::detour xmodel_get_bone_index_hook;
		__int64 xmodel_get_bone_index_stub(game::XModel* model, game::scr_string_t name, unsigned int offset, char* index)
		{
			auto result = xmodel_get_bone_index_hook.invoke<__int64>(model, name, offset, index);
			if (!result)
			{
				if (name == game::SL_GetString("tag_weapon_right", 0) ||
					name == game::SL_GetString("tag_knife_attach", 0))
				{
					result = xmodel_get_bone_index_hook.invoke<__int64>(model, game::SL_GetString("tag_weapon", 0), offset, index);
					if (result)
					{
						console::debug("using tag_weapon instead of %s (%s, %d)\n", game::SL_ConvertToString(name), model->name, offset);
					}
				}
			}
			return result;
		}

		void cw_mismatch_error_stub(int, const char* msg, ...)
		{
			char buffer[0x100];

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

			va_end(ap);

			console::error(buffer);
		}

		int g_find_config_string_index_stub(const char* string, int start, int max, int create, const char* errormsg)
		{
			create = 1;
			return utils::hook::invoke<int>(0x8B530_b, string, start, max, create, errormsg); // G_FindConfigstringIndex
		}

		template <typename T>
		void set_weapon_field(const std::string& weapon_name, unsigned int field, T value)
		{
			auto weapon = game::DB_FindXAssetHeader(game::ASSET_TYPE_WEAPON, weapon_name.data(), false).data;
			if (weapon)
			{
				if (field && field < (0xE20 + sizeof(T)))
				{
					*reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(weapon) + field) = value;
				}
				else
				{
					console::warn("weapon field: %d is higher than the size of weapon struct!\n", field);
				}
			}
			else
			{
				console::warn("weapon %s not found!\n", weapon_name.data());
			}
		}

		void set_weapon_field_float(const std::string& weapon_name, unsigned int field, float value)
		{
			set_weapon_field<float>(weapon_name, field, value);
		}

		void set_weapon_field_int(const std::string& weapon_name, unsigned int field, int value)
		{
			set_weapon_field<int>(weapon_name, field, value);
		}

		void set_weapon_field_bool(const std::string& weapon_name, unsigned int field, bool value)
		{
			set_weapon_field<bool>(weapon_name, field, value);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_sp())
			{
				// precache all weapons that are loaded in zones
				g_setup_level_weapon_def_hook.create(0x462630_b, g_setup_level_weapon_def_stub);

#ifdef DEBUG
				// use tag_weapon if tag_weapon_right or tag_knife_attach are not found on model
				xmodel_get_bone_index_hook.create(0x5C82B0_b, xmodel_get_bone_index_stub);
#endif
				// make custom weapon index mismatch not drop in CG_SetupCustomWeapon
				utils::hook::call(0x11B9AF_b, cw_mismatch_error_stub);

				// patch attachment configstring so it will create if not found
				utils::hook::call(0x41C595_b, g_find_config_string_index_stub);
			}

#ifdef DEBUG
			command::add("setWeaponFieldFloat", [](const command::params& params)
			{
				if (params.size() <= 3)
				{
					console::info("usage: setWeaponFieldInt <weapon> <field> <value>\n");
					return;
				}
				set_weapon_field_float(params.get(1), atoi(params.get(2)), static_cast<float>(atof(params.get(3))));
			});

			command::add("setWeaponFieldInt", [](const command::params& params)
			{
				if (params.size() <= 3)
				{
					console::info("usage: setWeaponFieldInt <weapon> <field> <value>\n");
					return;
				}
				set_weapon_field_int(params.get(1), atoi(params.get(2)), static_cast<int>(atoi(params.get(3))));
			});

			command::add("setWeaponFieldBool", [](const command::params& params)
			{
				if (params.size() <= 3)
				{
					console::info("usage: setWeaponFieldBool <weapon> <field> <value>\n");
					return;
				}
				set_weapon_field_bool(params.get(1), atoi(params.get(2)), static_cast<bool>(atoi(params.get(3))));
			});
#endif
		}
	};
}

REGISTER_COMPONENT(weapon::component)