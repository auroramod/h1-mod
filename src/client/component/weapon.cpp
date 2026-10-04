#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "weapon.hpp"

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

		utils::hook::detour xmodel_get_bone_index_hook;

		utils::memory::allocator ddl_allocator;

		/*
			1.04 - 128 camos, idx 0-7, model variant 8, camo 9-15, reticle 16-21, attachment combo 22-30
			1.15 - 512 camos, idx 0-7, camo 8-16, attachment combo 17-26 (no variant or reticle)
		*/
		constexpr std::uint32_t camo_shift = 8;
		constexpr std::uint32_t camo_mask = 0x1FF;
		constexpr std::uint32_t max_camos = 0x200;

		std::int8_t camo_flags[max_camos]{};
		void* camo_materials[max_camos]{};
		std::unordered_set<void*> modified_enums;

		int camo_table_get_id_stub(const char* value)
		{
			const auto id = std::atoi(value);
			return id > static_cast<int>(max_camos) ? 0 : id;
		}

		std::uint32_t get_weapon_camo(const std::uint32_t weapon)
		{
			return (weapon >> camo_shift) & camo_mask;
		}

		void get_weapon_model_info_stub(const std::uint32_t weapon, bool /*alt*/, int* variant, std::uint32_t* camo, int* emblem)
		{
			*variant = 0;
			*camo = get_weapon_camo(weapon);
			*emblem = utils::hook::invoke<int>(0x1401F8610, weapon);
		}

		std::uint32_t weapon_to_stream_key_stub(const std::uint32_t weapon)
		{
			auto key = ((weapon >> 14) & 0x1FF00) | (weapon & 0xFF);
			if (const auto camo = get_weapon_camo(weapon); camo && camo_flags[camo - 1] >= 0)
			{
				key |= 1 << 17;
			}

			return key;
		}

		std::uint32_t stream_key_to_weapon_stub(const std::uint32_t key)
		{
			auto weapon = ((key & 0x1FF00) << 14) | (key & 0xFF);
			if (key & (1 << 17))
			{
				const auto default_camo = *reinterpret_cast<std::uint32_t*>(0x146520604);
				weapon |= (default_camo & camo_mask) << camo_shift;
			}

			return weapon;
		}

		char* append_camo_name(char* dest, const std::uint32_t weapon, const char separator)
		{
			auto camo = get_weapon_camo(weapon);
			if (!camo)
			{
				return dest;
			}

			*dest++ = separator;
			std::memcpy(dest, "camo", 4);
			dest += 4;

			dest[2] = static_cast<char>('0' + camo % 10);
			camo /= 10;
			dest[1] = static_cast<char>('0' + camo % 10);
			dest[0] = static_cast<char>('0' + camo / 10);
			return dest + 3;
		}

		void patch_camo_call(const std::uintptr_t address, const std::size_t size, const std::function<void(utils::hook::assembler&)>& body)
		{
			utils::hook::nop(address, size);
			utils::hook::call(address, utils::hook::assemble([&](utils::hook::assembler& a)
			{
				body(a);
				a.ret();
			}));
		}

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
			return utils::hook::invoke<int>(0x1400731B0, string, start, max, create, errormsg); // G_FindConfigstringIndex
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
				const auto hash = utils::hook::invoke<unsigned int>(0x1406EA130, entries[i], 0);
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
				return utils::hook::invoke<void>(0x1402C1620, root);
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

			utils::hook::invoke<void>(0x1402C1620, asset);
		}

		void patch_num_weapons_reg()
		{
			// movzx edx, bl -> mov edx, ebx
			utils::hook::set<std::uint16_t>(0x1400C4770, 0xD38B);
			utils::hook::nop(0x1400C4772, 1);

			// (bunch of stuff) -> inc ebx
			utils::hook::set<std::uint16_t>(0x1400C47DD, 0xC3FF);
			utils::hook::nop(0x1400C47DF, 8);
			// movzx r8d, bl -> mov r8d, ebx
			utils::hook::set<std::uint32_t>(0x1400C47E7, 0x90C38B44);

			// (bunch of stuff) -> inc ebx
			utils::hook::set<std::uint16_t>(0x1403407B2, 0xC3FF);
			utils::hook::nop(0x1403407B4, 8);
			// movzx edi, bl -> mov edi, ebx
			utils::hook::set<std::uint16_t>(0x1403407BC, 0xDF89);
			utils::hook::nop(0x1403407BE, 1);

			utils::hook::set<std::uint16_t>(0x1401F7D09, 0xC3FF);
			utils::hook::nop(0x1401F7D0B, 3);
			utils::hook::nop(0x1401F7D10, 5);
			utils::hook::set<std::uint16_t>(0x1401F7D15, 0xD889);
			utils::hook::nop(0x1401F7D17, 1);
		}

		void patch_camo_bits()
		{
			using namespace asmjit::x86;

			// camo tables (sub_14038C9E0 loads mp/camoTable.csv)
			const auto flags = reinterpret_cast<std::uintptr_t>(camo_flags);
			const auto materials = reinterpret_cast<std::uintptr_t>(camo_materials);
			for (const auto address : {0x14038CA0D, 0x14038CB39, 0x14038CB6E, 0x140050F96, 0x1400510B6, 0x1400511A3, 0x140051236, 0x140051346, 0x1400CBCBB})
			{
				utils::hook::inject(address + 3, flags);
			}

			for (const auto address : {0x14038CA21, 0x14038D8C3})
			{
				utils::hook::inject(address + 3, materials);
			}

			utils::hook::set<std::int32_t>(0x14038D61A + 4, static_cast<std::int32_t>(materials - 0x140000000));
			utils::hook::set<std::int32_t>(0x14038D640 + 4, static_cast<std::int32_t>(materials - 0x140000000));
			utils::hook::set<std::int32_t>(0x14038D660 + 5, static_cast<std::int32_t>(flags - 0x140000000));
			utils::hook::set<std::uint32_t>(0x14038CA03 + 1, max_camos); // rep stosb count
			utils::hook::set<std::uint8_t>(0x14038CA2F + 4, max_camos / 8 - 1); // material clear loop
			utils::hook::call(0x14038CB12, camo_table_get_id_stub);

			// shr 9, and 7Fh -> shr 8, and 1FFh
			for (const auto& [address, reg] : {std::pair{0x140050F8C, ecx}, {0x1400510AC, ecx}, {0x140051196, eax}, {0x14005122C, ecx}, {0x140051339, eax}})
			{
				patch_camo_call(address, 6, [reg](utils::hook::assembler& a)
				{
					a.shr(reg, camo_shift);
					a.and_(reg, camo_mask);
				});
			}

			// Scr_GetWeaponCamoName
			patch_camo_call(0x1403588EA, 13, [](utils::hook::assembler& a)
			{
				a.shr(eax, camo_shift);
				a.and_(eax, camo_mask);
				a.mov(r8, 0x1408532C0); // "camo%02d"
			});

			// camo material apply (sub_14038D5B0)
			patch_camo_call(0x14038D69E, 11, [](utils::hook::assembler& a)
			{
				a.shr(r10d, camo_shift);
				a.mov(r9d, edi);
				a.and_(r10d, camo_mask);
			});

			patch_camo_call(0x14038D6B0, 11, [](utils::hook::assembler& a)
			{
				a.mov(edx, dword_ptr(rcx, -4));
				a.mov(eax, edx);
				a.shr(eax, camo_shift);
				a.and_(eax, camo_mask);
			});

			// weapon model variant + camo getters
			utils::hook::jump(0x140201650, get_weapon_model_info_stub);
			utils::hook::jump(0x140201710, get_weapon_model_info_stub);

			// weapon stream keys
			utils::hook::jump(0x14041B800, weapon_to_stream_key_stub);
			utils::hook::jump(0x14041BA40, stream_key_to_weapon_stub);

			// BG_GetWeaponNameComplete camo suffix (far jump clobbers rax, dest is also in r8)
			utils::hook::jump(0x1401F9778, utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.mov(rcx, r8);
				a.mov(edx, r9d);
				a.movzx(r8d, bpl);
				a.call_aligned(append_camo_name);
				a.mov(r8, rax);
				a.jmp(0x1401F97B7);
			}), true);

			// G_GetWeaponForName: camo clamp 7Fh -> 1FFh, reticle clamp 40h -> 20h, pack ((reticle << 9) | camo) << 8
			patch_camo_call(0x14038C485, 7, [](utils::hook::assembler& a)
			{
				a.cmp(cx, camo_mask);
				a.mov(r15d, r9d);
			});

			utils::hook::set<std::uint8_t>(0x14038C4E3 + 3, 0x20);
			utils::hook::set<std::uint8_t>(0x14038C619 + 2, 0x1F);
			utils::hook::nop(0x14038C628, 3);
			utils::hook::set<std::uint8_t>(0x14038C62B + 2, 9);
			utils::hook::set<std::uint8_t>(0x14038C630 + 2, camo_shift);

			// give weapon script camo/reticle params (0x14032EA90)
			patch_camo_call(0x14032EB84, 19, [](utils::hook::assembler& a)
			{
				a.cmp(eax, camo_mask);
				a.cmovge(eax, r13d);
				a.shl(eax, camo_shift);
				a.xor_(eax, ebx);
				a.and_(eax, camo_mask << camo_shift);
				a.xor_(ebx, eax);
			});

			utils::hook::set<std::uint8_t>(0x14032EC10 + 2, 0x1F);
			utils::hook::set<std::uint8_t>(0x14032EC17 + 2, 0x11);
			utils::hook::set<std::uint32_t>(0x14032EC1C + 1, 0x3E0000);

			// patch reticles   shr 10h, and 3Fh -> shr 11h, and 1Fh
			utils::hook::set<std::uint8_t>(0x1401F8270 + 2, 0x11);
			utils::hook::set<std::uint8_t>(0x1401F8273 + 2, 0x1F);
			utils::hook::set<std::uint8_t>(0x1401F97BC + 3, 0x11);
			utils::hook::set<std::uint8_t>(0x1401F97C0 + 3, 0x1F);
			utils::hook::set<std::uint8_t>(0x1401FA5A0 + 3, 0x11);
			utils::hook::set<std::uint8_t>(0x1401FA5A4 + 2, 0x1F);

			// patch model variants   and 1 -> and 0 (bit 8 is camo now)
			utils::hook::set<std::uint8_t>(0x1400E81FD + 2, 0);
			utils::hook::set<std::uint8_t>(0x1402016F7 + 2, 0);
			utils::hook::set<std::uint8_t>(0x1402017BA + 2, 0);
			utils::hook::set<std::uint8_t>(0x1402203AF + 3, 0);
			utils::hook::set<std::uint8_t>(0x140329B0A + 2, 0);

			// patch BG_PlayerSetWeaponModelVariant (sub_140202340) to never write bit 8
			utils::hook::nop(0x14020235A, 2);
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
				g_setup_level_weapon_def_hook.create(0x14038D240, g_setup_level_weapon_def_stub);

				// use tag_weapon if tag_weapon_right or tag_knife_attach are not found on model
				xmodel_get_bone_index_hook.create(0x14051E0C0, xmodel_get_bone_index_stub);

				// make custom weapon index mismatch not drop in CG_SetupCustomWeapon
				utils::hook::call(0x1400C708F, cw_mismatch_error_stub);

				// patch attachment configstring so it will create if not found
				utils::hook::call(0x14033F1F5, g_find_config_string_index_stub);

				utils::hook::call(0x140291154, load_ddl_asset_stub);

				dvars::register_bool("sv_disableCustomClasses", 
					false, game::DVAR_CODINFO, "Disable custom classes on server");

				patch_camo_bits();			// use the 1.15 weapon camo layout (9 bit)
				patch_num_weapons_reg();	// change register used for BG_GetNumWeapons loops to 32 bits
			}

#ifdef _DEBUG
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
