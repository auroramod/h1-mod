#include <std_include.hpp>

#ifdef DEBUG
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/scheduler.hpp"
#include "component/command.hpp"
#include "component/fastfiles.hpp"
#include "../gui.hpp"
#include "../asset_list.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>

namespace gui::asset_list::sound
{
	namespace
	{
#define DRAW_ASSET_FLOAT_SLIDER(__name__, __property__, __min__, __max__, __step__) \
		ImGui::DragFloat(__name__, &asset->__property__, __step__, __min__, __max__); \

#define DRAW_ASSET_PROPERTY(__name__, __fmt__) \
		ImGui::Text(#__name__ ": " __fmt__, asset->__name__); \

#define DRAW_ASSET_PROPERTY_COPY(__name__) \
		ImGui::Text(#__name__ ": "); \
		ImGui::SameLine(); \
		if (ImGui::Button(asset->__name__)) \
		{ \
			gui::copy_to_clipboard(asset->__name__); \
		} \

		std::array<const char*, game::SND_VOLMOD_COUNT> volume_mod_groups =
		{
			"default",
			// User interface
			"hud",
			"frontend_sfx",
			"frontend_music",
			"sp_hit_alert_npc",
			// Music
			"music",
			"music2",
			"music_big",
			"music_emitter",
			// Ambience
			"amb_quad",
			"amb_dist",
			"amb_damb",
			"amb_damb_big",
			"amb_emitter",
			"amb_emitter_int",
			"amb_emitter_ext",
			// Physics
			"physics",
			// Characters
			"bodyfall",
			"bodyfall_mech",
			"foley_plr",
			"foley_plr_special",
			"mp_foley_plr",
			"foley_npc",
			"foley_mech",
			"foleymp_npc",
			"wpn_foley_plr",
			"wpn_foley_npc",
			"wpn_foley_mech",
			"footstep_plr",
			"footstep_plr_special",
			"footstep_water_plr",
			"footstep_npc",
			"footstep_mech",
			"mp_footstep_plr",
			"mp_footstep_npc",
			"melee_plr",
			"melee_npc",
			"melee_hit",
			"plr_internal",
			"deaths_door",
			// Dialogue
			"chatter_friendly",
			"chatter_enemy",
			"chatter_atlas",
			"vox_friendly",
			"vox_enemy",
			"dialog",
			"dialog_radio",
			"dialog_critical",
			"dialog_amb",
			// Explosions & Destruction
			"destruct",
			"explosion",
			"explosion_grenade",
			"explosion_flashbang",
			"explosion_rocket",
			"explosion_car",
			"explosion_critical",
			"explosion_debris",
			"impact_critical",
			// Bullet Impacts & Whizbys
			"bullet_impact_geo",
			"bullet_impact_plr",
			"bullet_impact_npc",
			"mp_bullet_impact_geo",
			"mp_bullet_impact_plr",
			"mp_bullet_impact_npc",
			"bullet_whizby",
			"mp_bullet_whizby",
			"bullet_impact_special",
			// Vehicles
			"vehicle_plr",
			"vehicle_plr_special",
			"vehicle_npc",
			"vehicle_npc_special",
			"vehicle_avm_plr",
			"vehicle_avm_npc",
			"wpn_vehicle_plr",
			"wpn_vehicle_npc",
			"vehicle",
			"vehicle_aircraft",
			"vehicle_pdrone",
			"vehicle_drone_swarm",
			"vehicle_amb",
			// Weapons
			"grenadebounce",
			"mp_grenadebounce",
			"shellcasings",
			"mp_shellcasings",
			"wpn_plr",
			"wpn_plr_special",
			"wpn_npc_special",
			"mp_wpn_plr",
			"wpn_npc",
			"mp_wpn_npc",
			"wpn_projectile",
			"mp_wpn_projectile",
			// Weapons Detailed
			"wpn_plr_shot_special",
			"wpn_plr_shot",
			"wpn_plr_mech",
			"wpn_plr_sub",
			"wpn_plr_tail",
			"wpn_npc_shot",
			"wpn_npc_mech",
			"wpn_npc_sub",
			"wpn_npc_tail",
			"wpn_npc_dist",
			// Tactical
			"tactical_plr",
			"tactical_npc",
			"tactical_plr_special",
			"tactical_npc_special",
			// Attack Dogs
			"attack_dog",
			// Special use
			"big_moment",
			"scripted1",
			"scripted2",
			"scripted3",
			"scripted4",
			"scripted5",
			"scripted6",
			"scripted7",
			"scripted8",
			"scripted9",
			"scripted10",
			"fullvolume",
			"mp_pre_event",
			"mp_post_event",
			"mp_splash_notify",
			"mp_hit_marker",
			// Level specific and MP perks
			"mp_perk_quiet",
			// OLD
			"mw3_amb_damb",
			"mw3_amb_emitter",
			"mw3_amb_quad",
			"mw3_bodyfall",
			"mw3_bullet_impact_geo",
			"mw3_bullet_impact_npc",
			"mw3_bullet_impact_plr",
			"mw3_bullet_whizby",
			"mw3_chatter_enemy",
			"mw3_chatter_friendly",
			"mw3_DEPRECATED_default",
			"mw3_DEPRECATED_MAX",
			"mw3_DEPRECATED_na",
			"mw3_destruct",
			"mw3_dialog",
			"mw3_dialog_amb",
			"mw3_dialog_critical",
			"mw3_dialog_radio",
			"mw3_explosion",
			"mw3_explosion_car",
			"mw3_explosion_flashbang",
			"mw3_explosion_grenade",
			"mw3_explosion_rocket",
			"mw3_foley_npc",
			"mw3_footstep_npc",
			"mw3_frontend_sfx",
			"mw3_fullvolume",
			"mw3_grenadebounce",
			"mw3_melee_npc",
			"mw3_mp_bullet_impact_geo",
			"mw3_mp_bullet_impact_npc",
			"mw3_mp_foley_npc",
			"mw3_mp_footstep_npc",
			"mw3_mp_wpn_npc",
			"mw3_music",
			"mw3_physics",
			"mw3_scripted1",
			"mw3_scripted2",
			"mw3_scripted3",
			"mw3_scripted4",
			"mw3_scripted5",
			"mw3_shellcasings",
			"mw3_vehicle",
			"mw3_vehicle_plr",
			"mw3_vehicle_npc",
			"mw3_vox_enemy",
			"mw3_vox_friendly",
			"mw3_wpn_foley_npc",
			"mw3_wpn_foley_plr",
			"mw3_footstep_plr",
			"mw3_wpn_npc",
			"mw3_wpn_plr",
			"mw3_wpn_projectile",
			"mw3_wpn_vehicle_npc",
			"mw3_wpn_vehicle_plr",
		};

		std::array<const char*, game::SND_DSPBUS_COUNT> dsp_buses =
		{
			"default",
			"weapons",
			"weapons_dist",
			"ambience",
			"emitters",
			"quads",
			"music",
			"foley",
			"explosions",
			"explosions_critical",
			"tactical",
			"tactical_special",
			"vehicles",
			"air_vehicles",
			"int_vehicles_1",
			"int_vehicles_2",
			"voices",
			"voices_critical",
			"voices_unfiltered",
			"animals",
			"physics",
			"battlechatter",
			"interface",
			"scripted",
			"scripted1",
			"scripted2",
			"scripted3",
			"scripted4",
			"scripted5",
			"unoccluded",
			"whizbys",
			"notimescale",
		};

		bool draw_sound_window(game::snd_alias_list_t* asset)
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);

			if (ImGui::TreeNode("aliases"))
			{
				for (auto i = 0; i < asset->count; i++)
				{
					DRAW_ASSET_PROPERTY_COPY(head[i].aliasName);
					DRAW_ASSET_FLOAT_SLIDER("volMin", head[i].volMin, 0.0f, 2.0f, 0.05f);
					DRAW_ASSET_FLOAT_SLIDER("volMax", head[i].volMax, 0.0f, 2.0f, 0.05f);
					DRAW_ASSET_FLOAT_SLIDER("pitchMin", head[i].pitchMin, 0.0f, 2.0f, 0.05f);
					DRAW_ASSET_FLOAT_SLIDER("pitchMax", head[i].pitchMax, 0.0f, 2.0f, 0.05f);
					DRAW_ASSET_FLOAT_SLIDER("distMin", head[i].distMin, -20000.0f, 20000.0f, 10.0f);
					DRAW_ASSET_FLOAT_SLIDER("distMax", head[i].distMax, -20000.0f, 20000.0f, 10.0f);

					DRAW_ASSET_PROPERTY(head[i].flags, "%i");
                    auto* flags = reinterpret_cast<game::SoundAliasFlags*>(&asset->head[i].flags);
                    ImGui::Text("probability: %f", asset->head[i].probability);
                    ImGui::Text("looping: %i", flags->packed.looping);
                    ImGui::Text("isMaster: %i", flags->packed.isMaster);
                    ImGui::Text("isSlave: %i", flags->packed.isSlave);
                    ImGui::Text("fullDryLevel: %i", flags->packed.fullDryLevel);
                    ImGui::Text("noWetLevel: %i", flags->packed.noWetLevel);
                    ImGui::Text("randomLooping: %i", flags->packed.randomLooping);
                    ImGui::Text("spatializedRangeCheck: %i", flags->packed.spatializedRangeCheck);
                    ImGui::Text("spatializedIs3D: %i", flags->packed.spatializedIs3D);
                    ImGui::Text("unk9: %i", flags->packed.unk9);
                    ImGui::Text("inheritPitch: %i", flags->packed.inheritPitch);
                    ImGui::Text("inheritVolume: %i", flags->packed.inheritVolume);
                    ImGui::Text("useContextList: %i", flags->packed.useContextList);
                    ImGui::Text("useNoPanning2D: %i", flags->packed.useNoPanning2D);
                    ImGui::Text("useOldPanning: %i", flags->packed.useOldPanning);
                    ImGui::Text("useNoPanning3D: %i", flags->packed.useNoPanning3D);
                    ImGui::Text("type: %i", flags->packed.type);
                    ImGui::Text("unused: %i", flags->packed.unused);

					ImGui::Text("dspBus: %s", dsp_buses[asset->head[i].dspBusIndex]);
					ImGui::Text("volMod: %s", volume_mod_groups[asset->head[i].volModIndex]);

					ImGui::Separator();
				}
				ImGui::TreePop();
			}
			return true;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			gui::asset_list::add_asset_view<game::snd_alias_list_t>(game::ASSET_TYPE_SOUND, draw_sound_window);
		}
	};
}

REGISTER_COMPONENT(gui::asset_list::sound::component)
#endif
