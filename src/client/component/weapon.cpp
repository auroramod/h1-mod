#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "console.hpp"
#include "fastfiles.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/memory.hpp>

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
				//console::debug("precaching weapon \"%s\"\n", weapons[i]->name);
				game::G_GetWeaponForName(weapons[i]->name);
			}
		}

		utils::hook::detour xmodel_get_bone_index_hook;
		int xmodel_get_bone_index_stub(game::XModel* model, game::scr_string_t name, unsigned int offset, char* index)
		{
			auto result = xmodel_get_bone_index_hook.invoke<int>(model, name, offset, index);
			if (result)
			{
				return result;
			}

			const auto original_index = *index;
			const auto original_result = result;

			if (name == game::SL_FindString("tag_weapon_right") ||
				name == game::SL_FindString("tag_knife_attach"))
			{
				const auto tag_weapon = game::SL_FindString("tag_weapon");
				result = xmodel_get_bone_index_hook.invoke<int>(model, tag_weapon, offset, index);
				if (result)
				{
					console::debug("using tag_weapon instead of %s (%s, %d, %d)\n", game::SL_ConvertToString(name), model->name, offset, *index);
					return result;
				}
			}

			*index = original_index;
			result = original_result;

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

		int compare_hash(const void* a, const void* b)
		{
			const auto hash_a = reinterpret_cast<game::DDLHash*>(
				reinterpret_cast<size_t>(a))->hash;
			const auto hash_b = reinterpret_cast<game::DDLHash*>(
				reinterpret_cast<size_t>(b))->hash;

			if (hash_a < hash_b)
			{
				return -1;
			}
			else if (hash_a > hash_b)
			{
				return 1;
			}

			return 0;
		}

		utils::memory::allocator ddl_allocator;
		std::unordered_set<void*> modified_enums;

		std::vector<const char*> get_stringtable_entries(const std::string& name)
		{
			std::vector<const char*> entries;

			const auto string_table = game::DB_FindXAssetHeader(
				game::ASSET_TYPE_STRINGTABLE, name.data(), false).stringTable;

			if (string_table == nullptr)
			{
				return entries;
			}

			for (auto row = 0; row < string_table->rowCount; row++)
			{
				if (string_table->columnCount <= 0)
				{
					continue;
				}

				const auto index = (row * string_table->columnCount);
				const auto weapon = string_table->values[index].string;
				entries.push_back(ddl_allocator.duplicate_string(weapon));
			}

			return entries;
		}

		void add_entries_to_enum(game::DDLEnum* enum_, const std::vector<const char*> entries)
		{
			if (entries.size() <= 0)
			{
				return;
			}

			const auto new_size = enum_->memberCount + entries.size();
			const auto members = ddl_allocator.allocate_array<const char*>(new_size);
			const auto hash_list = ddl_allocator.allocate_array<game::DDLHash>(new_size);

			std::memcpy(members, enum_->members, 8 * enum_->memberCount);
			std::memcpy(hash_list, enum_->hashTable.list, 8 * enum_->hashTable.count);

			for (auto i = 0; i < entries.size(); i++)
			{
				const auto hash = utils::hook::invoke<unsigned int>(0x794FB0_b, entries[i], 0);
				const auto index = enum_->memberCount + i;
				hash_list[index].index = index;
				hash_list[index].hash = hash;
				members[index] = entries[i];
			}

			std::qsort(hash_list, new_size, sizeof(game::DDLHash), compare_hash);

			enum_->members = members;
			enum_->hashTable.list = hash_list;
			enum_->memberCount = static_cast<int>(new_size);
			enum_->hashTable.count = static_cast<int>(new_size);
		}

		void load_ddl_asset_stub(game::DDLRoot** asset)
		{
			const auto root = *asset;
			if (!root->ddlDef)
			{
				return utils::hook::invoke<void>(0x39BE20_b, root);
			}

			auto ddl_def = root->ddlDef;
			while (ddl_def)
			{
				for (auto i = 0; i < ddl_def->enumCount; i++)
				{
					const auto enum_ = &ddl_def->enumList[i];
					if (modified_enums.contains(enum_))
					{
						continue;
					}

					if ((enum_->name == "WeaponStats"s || enum_->name == "Weapon"s))
					{
						const auto weapons = get_stringtable_entries("mp/customweapons.csv");
						add_entries_to_enum(enum_, weapons);
						modified_enums.insert(enum_);
					}

					if (enum_->name == "AttachmentBase"s)
					{
						const auto attachments = get_stringtable_entries("mp/customattachments.csv");
						add_entries_to_enum(enum_, attachments);
						modified_enums.insert(enum_);
					}
				}

				ddl_def = ddl_def->next;
			}

			utils::hook::invoke<void>(0x39BE20_b, asset);
		}
	}

	void clear_modifed_enums()
	{
		modified_enums.clear();
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

				// use tag_weapon if tag_weapon_right or tag_knife_attach are not found on model
				xmodel_get_bone_index_hook.create(0x5C82B0_b, xmodel_get_bone_index_stub);
				// make custom weapon index mismatch not drop in CG_SetupCustomWeapon
				utils::hook::call(0x11B9AF_b, cw_mismatch_error_stub);

				// patch attachment configstring so it will create if not found
				utils::hook::call(0x41C595_b, g_find_config_string_index_stub);

				utils::hook::call(0x36B4D4_b, load_ddl_asset_stub);

				dvars::register_bool("sv_disableCustomClasses", 
					false, game::DVAR_FLAG_REPLICATED, "Disable custom classes on server");
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