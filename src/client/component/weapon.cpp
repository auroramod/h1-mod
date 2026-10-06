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

		/*
			1.04 - 128 camos, idx 0-7, model variant 8, camo 9-15, reticle 16-21, attachment combo 22-30
			1.15 - 512 camos, idx 0-7, camo 8-16, attachment combo 17-26 (no variant or reticle)
		*/
		constexpr std::uint32_t camo_shift = 8;
		constexpr std::uint32_t camo_mask = 0x1FF;
		constexpr std::uint32_t max_camos = 0x200;

		std::int8_t camo_flags[max_camos]{};
		void* camo_materials[max_camos]{};

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

		std::uint8_t get_weapon_index(const game::Weapon weapon)
		{
			return static_cast<std::uint8_t>(weapon.data);
		}

		game::WeaponDef* get_weapon_def(const game::Weapon weapon)
		{
			const auto weapon_defs = utils::hook::extract<game::WeaponDef**>(reinterpret_cast<void*>(0x1400A8450 + 3));
			return weapon_defs[get_weapon_index(weapon)];
		}

		bool is_melee_weapon(const game::Weapon weapon)
		{
			return get_weapon_def(weapon)->inventoryType == game::WEAPINVENTORY_MELEE;
		}

		// 1.15 sub_2E67D0
		game::Weapon get_melee_weapon(const game::playerState_s& ps)
		{
			for (const auto& weapon : ps.weaponsEquipped)
			{
				if (get_weapon_index(weapon) && is_melee_weapon(weapon))
				{
					return weapon;
				}
			}

			return {};
		}

		// 1.15 sub_2E8DA0
		game::XModel* get_melee_knife_model(const game::Weapon weapon, const game::Weapon melee)
		{
			if (get_weapon_index(melee) && get_weapon_index(melee) != get_weapon_index(weapon))
			{
				if (game::BG_HasAttachmentCombo(melee))
				{
					const char* models[2]{};
					if (utils::hook::invoke<int>(0x140051070, melee, models, 2) > 0) // returns view model count
					{
						return game::DB_FindXAssetHeader(game::ASSET_TYPE_XMODEL, models[0], 1).model;
					}
				}
				else
				{
					return game::BG_GetGunModel(melee, false, 0);
				}
			}

			return game::BG_GetKnifeModel(weapon, false);
		}

		// 1.15 sub_1E9280 + sub_2E8DA0
		game::XModel* get_view_knife_model(const game::Weapon weapon, const int hand)
		{
			if (!game::BG_GetKnifeModel(weapon, false))
			{
				return nullptr;
			}

			const auto& ps = game::cgameGlob->predictedPlayerState;
			if (!get_weapon_def(weapon)->knifeAlwaysAttached)
			{
				const auto state = ps.weaponState[hand].weaponState & ~0x800;
				if (state < 10 || state > 16 || is_melee_weapon(weapon))
				{
					return nullptr;
				}
			}

			return get_melee_knife_model(weapon, get_melee_weapon(ps));
		}

		game::Weapon view_melee_weapon{};
		bool view_anims_alt = false;

		// 1.15 sub_2E92E0
		bool apply_melee_weapon_anims(const bool alt, game::XAnimParts* (*anims)[game::NUM_WEAP_ANIMS])
		{
			view_anims_alt = alt;

			const auto& ps = game::cgameGlob->predictedPlayerState;
			const auto melee = get_melee_weapon(ps);
			if (!get_weapon_index(melee) || get_weapon_index(melee) == get_weapon_index(ps.weapCommon.weapon))
			{
				return alt;
			}

			// view anim <- melee weapon anim
			constexpr std::pair<game::weapAnimFiles_t, game::weapAnimFiles_t> melee_anims[] =
			{
				{game::WEAP_ANIM_MELEE_SWIPE, game::WEAP_ANIM_MELEE_ALT_STANDING},
				{game::WEAP_ANIM_MELEE_FATAL, game::WEAP_ANIM_MELEE_ALT_CROUCHING},
			};

			const auto melee_xanims = get_weapon_def(melee)->szXAnims;
			auto applied = false;

			for (const auto& [slot, source] : melee_anims)
			{
				if (!melee_xanims[source])
				{
					continue;
				}

				for (auto set = 0; set < 4; set++)
				{
					if (anims[set][slot])
					{
						anims[set][slot] = melee_xanims[source];
					}
				}

				applied = true;
			}

			return alt || applied;
		}

		bool has_underbarrel_ammo_stub(const game::Weapon weapon)
		{
			return game::BG_HasUnderbarrelAmmo(weapon) || !view_anims_alt;
		}

		// 1.15 sub_119960
		void update_view_weapon_info(const int local_client_num, game::playerState_s* ps, const int flags,
			const game::Weapon weapon, game::ViewModelInfo* info, const bool force)
		{
			if (!get_weapon_index(weapon))
			{
				return;
			}

			const auto melee = get_melee_weapon(*ps);
			const auto keep = !force && melee.data == view_melee_weapon.data &&
				(info->weapon.data == weapon.data || utils::hook::invoke<bool>(0x140203AC0, ps, flags, info->weapon, weapon)); // same viewmodel weapon

			if (!keep)
			{
				utils::hook::invoke<void>(0x1400A8430, local_client_num, weapon, info); // load weapon viewmodel info
			}

			if (!keep || (!info->handModel && !info->numExtraModels))
			{
				info->handModel = get_weapon_def(weapon)->handModel;
			}

			info->weapon = weapon;
			view_melee_weapon = melee;
		}

		bool hide_view_weapon_stub(const bool hide)
		{
			return hide || (game::viewModelInfo->knifeModel && get_weapon_index(get_melee_weapon(game::cgameGlob->predictedPlayerState)));
		}

		// 1.15 CL_ExecuteKey case 109
		void weapmelee(const int local_client_num)
		{
			const auto weapon = game::cgameGlob->predictedPlayerState.weapCommon.weapon;
			utils::hook::invoke<void>(0x1400A8A10, local_client_num, 1, !is_melee_weapon(weapon)); // CG_CycleWeapon
		}

		// 1.15 PM_Weapon_FinishWeaponChange
		void finish_weapon_change_melee(const game::playerState_s* ps, int* change_type)
		{
			if (*change_type == 4 && is_melee_weapon(ps->weapCommon.weapon))
			{
				*change_type = 3; // switching away from the melee weapon uses the regular raise
			}
		}

		/*
			1.15 per weapon melee sounds and surface fx (mp/meleeWeaponData.csv)
			1.04 only has the default layout of 12 sound aliases and 53 surface entries
		*/
		constexpr auto max_melee_sets = 32;
		constexpr auto melee_set_size = 0x41;
		constexpr auto melee_surface_offset = 0xC;
		constexpr auto melee_surface_count = 0x35;

		game::snd_alias_list_t* melee_sets[max_melee_sets][melee_set_size]{};
		int melee_set_map[256]{};
		bool melee_event_local = false;

		game::snd_alias_list_t* find_sound_alias(const char* name)
		{
			return utils::hook::invoke<game::snd_alias_list_t*>(0x1404F7660, name); // Com_FindSoundAlias
		}

		void load_melee_surface_table(const char* name, game::snd_alias_list_t** dest)
		{
			utils::hook::invoke<void>(0x14023D880, name, dest, true); // loads a surface type sound table
		}

		void load_default_melee_set()
		{
			auto& set = melee_sets[0];
			set[0] = find_sound_alias("melee_knife_swipe_start_plr");
			set[1] = find_sound_alias("melee_knife_swipe_start_npc");
			set[2] = find_sound_alias("melee_knife_stab_upper_start_plr");
			set[3] = find_sound_alias("melee_knife_stab_upper_start_npc");
			set[4] = find_sound_alias("wpn_combatknife_stab_plr");
			set[5] = find_sound_alias("wpn_combatknife_stab_npc");
			set[6] = find_sound_alias("melee_knife_stab_upper_hit_plr");
			set[7] = find_sound_alias("melee_knife_stab_upper_hit_npc");
			set[8] = find_sound_alias("wpn_combatknife_plr");
			set[9] = find_sound_alias("wpn_combatknife_npc");
			set[10] = find_sound_alias("melee_knife_hit_other");
			set[11] = find_sound_alias("melee_knife_hit_shield");
			load_melee_surface_table("melee_knife_hit", &set[melee_surface_offset]);
		}

		// 1.15 sub_332DC0
		void load_melee_weapon_data()
		{
			std::memset(melee_sets, 0, sizeof(melee_sets));
			std::memset(melee_set_map, 0, sizeof(melee_set_map));

			game::StringTable* table = nullptr;
			game::StringTable_GetAsset("mp/meleeWeaponData.csv", &table);
			const auto rows = table ? game::StringTable_GetRowCount(table) : 0;
			if (rows <= 0)
			{
				load_default_melee_set();
				return;
			}

			const auto get_column = [&](const int row, const int column)
			{
				return game::StringTable_GetColumnValueForRow(table, row, column);
			};

			// set slot -> table column
			static const std::pair<int, int> alias_columns[] =
			{
				{4, 10}, {5, 11}, {10, 9}, {8, 7}, {9, 8}, {3, 5}, {2, 4}, {0, 2}, {1, 3}, {7, 13}, {6, 12},
			};

			for (auto row = 0; row < rows; row++)
			{
				const auto id_string = get_column(row, 0);
				if (*id_string < '0' || *id_string > '9')
				{
					continue;
				}

				const auto id = std::atoi(id_string);
				if (id >= max_melee_sets)
				{
					continue;
				}

				const auto weapon_name = get_column(row, 1);
				if (_stricmp(weapon_name, "default"))
				{
					const auto weapon = game::BG_FindWeaponForName(weapon_name);
					melee_set_map[get_weapon_index(weapon)] = id;
				}

				auto& set = melee_sets[id];
				for (const auto& [slot, column] : alias_columns)
				{
					set[slot] = find_sound_alias(get_column(row, column));
				}

				set[11] = find_sound_alias("melee_knife_hit_shield");
				load_melee_surface_table(get_column(row, 6), &set[melee_surface_offset]);
			}
		}

		game::snd_alias_list_t* get_melee_sound_alias(const game::Weapon weapon, const int slot)
		{
			return melee_sets[melee_set_map[get_weapon_index(weapon)]][slot];
		}

		game::snd_alias_list_t* get_melee_surface_alias(const game::Weapon weapon, const int type, const int surface)
		{
			return melee_sets[melee_set_map[get_weapon_index(weapon)]][melee_surface_offset + type * melee_surface_count + surface];
		}

		// 1.15 sub_F9400
		game::snd_alias_list_t* get_melee_event_sound_alias(const game::Weapon weapon, const int slot)
		{
			if (melee_event_local)
			{
				if (const auto melee = get_melee_weapon(game::cgameGlob->predictedPlayerState); get_weapon_index(melee))
				{
					return get_melee_sound_alias(melee, slot);
				}
			}

			return get_melee_sound_alias(weapon, slot);
		}

		void patch_melee_weapon()
		{
			// show the equipped melee weapon instead of the gun's knife model
			utils::hook::jump(0x1400F8DD8, utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.mov(ecx, ebx); // weapon
				a.mov(edx, r12d); // view index
				a.call_aligned(get_view_knife_model);
				a.mov(rsi, rax);
				a.jmp(0x1400F8E21);
			}), true);

			// melee viewanims from the melee weapon, rebuilt when it changes
			utils::hook::jump(0x1400C5760, update_view_weapon_info);
			utils::hook::call(0x1400A86C8, has_underbarrel_ammo_stub);
			utils::hook::call(0x1400A8713, has_underbarrel_ammo_stub);

			// hide our actual weapon when we knife
			utils::hook::nop(0x1400D390E, 9);
			utils::hook::jump(0x1400D390E, utils::hook::assemble([](utils::hook::assembler& a)
			{
				const auto unchanged = a.newLabel();

				a.movzx(ecx, r12b);
				a.call_aligned(hide_view_weapon_stub);
				a.mov(r12b, al);
				a.mov(rax, reinterpret_cast<std::size_t>(&game::viewModelInfo->hideWeapon));
				a.cmp(r12b, byte_ptr(rax));
				a.jz(unchanged);
				a.mov(byte_ptr(rax), r12b);
				a.jmp(0x1400D396B);

				a.bind(unchanged);
				a.jmp(0x1400D3967);
			}));

			utils::hook::nop(0x1400A8683, 8);
			utils::hook::jump(0x1400A8683, utils::hook::assemble([](utils::hook::assembler& a)
			{
				const auto copy_alt_only = a.newLabel();

				a.movzx(ecx, al);
				a.mov(rdx, r13); // anims
				a.call_aligned(apply_melee_weapon_anims);
				a.test(al, al);
				a.jz(copy_alt_only);
				a.jmp(0x1400A868B);

				a.bind(copy_alt_only);
				a.jmp(0x1400A871C);
			}));

			// CL_ExecuteKey, add weapmelee id 109 (1.4 stops at 107)
			utils::hook::nop(0x14024AD4F, 6);
			utils::hook::jump(0x14024AD4F, utils::hook::assemble([](utils::hook::assembler& a)
			{
				const auto out_of_table = a.newLabel();
				const auto default_case = a.newLabel();

				a.ja(out_of_table);
				a.jmp(0x14024AD55);

				a.bind(out_of_table);
				a.cmp(ebp, 0x6C);
				a.jne(default_case);

				a.mov(ecx, ebx);
				a.call_aligned(weapmelee);

				a.bind(default_case);
				a.jmp(0x14024BC45);
			}));

			// CycleWeapPrimary, 5th arg is cycle to the melee weapon
			utils::hook::nop(0x1400DC995, 9);
			utils::hook::jump(0x1400DC995, utils::hook::assemble([](utils::hook::assembler& a)
			{
				const auto melee = a.newLabel();

				a.cmp(dword_ptr(rsp, 0x80), edi);
				a.jnz(melee);
				a.jmp(0x1400DC99E);

				a.bind(melee);
				a.mov(r12d, game::WEAPINVENTORY_MELEE);
				a.jmp(0x1400DC9FA);
			}));

			// PM_Weapon_FinishWeaponChange
			utils::hook::nop(0x1401F182C, 7);
			utils::hook::jump(0x1401F182C, utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.pushad64();
				a.mov(rcx, rbp); // ps
				a.lea(rdx, qword_ptr(rsp, 0x80 + 0xB8)); // change type (arg 2)
				a.call_aligned(finish_weapon_change_melee);
				a.popad64();

				a.cmp(dword_ptr(rbp, 0x1DD0), 1);
				a.jmp(0x1401F1833);
			}));

			// melee weapon data table
			utils::hook::jump(0x14023C080, load_melee_weapon_data);
			utils::hook::jump(0x14023BB50, get_melee_sound_alias);
			utils::hook::jump(0x14023BB20, get_melee_surface_alias);

			// CG_EntityEvent
			utils::hook::nop(0x1400ACBE2, 6);
			utils::hook::jump(0x1400ACBE2, utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.mov(rax, reinterpret_cast<std::size_t>(&melee_event_local));
				a.mov(byte_ptr(rax), 1);
				a.mov(rax, 0x142935398);
				a.mov(edi, dword_ptr(rax));
				a.jmp(0x1400ACBE8);
			}));

			utils::hook::jump(0x1400ACC12, utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.mov(rax, reinterpret_cast<std::size_t>(&melee_event_local));
				a.mov(byte_ptr(rax), 0);
				a.cmp(byte_ptr(r15, 6), bl);
				a.mov(esi, dword_ptr(r15, 0x4C));
				a.jmp(0x1400ACC1A);
			}));

			// melee swing / hit events
			utils::hook::nop(0x1400AD7AB, 14);
			utils::hook::jump(0x1400AD7AB, utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.movzx(edx, r12b);
				a.xor_(edx, 1);
				a.mov(ecx, esi);
				a.call_aligned(get_melee_event_sound_alias);
				a.jmp(0x1400AD7CA);
			}));

			utils::hook::nop(0x1400AD821, 15);
			utils::hook::jump(0x1400AD821, utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.neg(r12b);
				a.sbb(edx, edx);
				a.add(edx, 3);
				a.mov(ecx, esi);
				a.call_aligned(get_melee_event_sound_alias);
				a.jmp(0x1400AD830);
			}));
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

				patch_camo_bits();			// use 1.15 weapon camo layout (9 bit)
				patch_num_weapons_reg();	// change register used for BG_GetNumWeapons loops to 32 bits
				patch_melee_weapon();		// add and use 1.15 melee weapon slot
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