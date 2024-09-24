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

        static const std::vector<std::string> sound_channels
        {
            "physics",
            "ambdist1",
            "ambdist2",
            "alarm",
            "auto",
            "auto2",
            "auto2d",
            "autodog",
            "explosiondist1",
            "explosiondist2",
            "explosiveimpact",
            "element",
            "element_int",
            "element_ext",
            "foley_plr_mvmt",
            "foley_plr_weap",
            "foley_npc_mvmt",
            "foley_npc_weap",
            "foley_dog_mvmt",
            "element_lim",
            "element2d",
            "voice_dog_dist",
            "bulletflesh1npc_npc",
            "bulletflesh2npc_npc",
            "bulletimpact",
            "bulletflesh1npc",
            "bulletflesh2npc",
            "bulletflesh1",
            "bulletflesh2",
            "vehicle",
            "vehiclelimited",
            "menu",
            "menulim1",
            "menulim2",
            "bulletwhizbyin",
            "bulletwhizbyout",
            "body",
            "body2d",
            "reload",
            "reload2d",
            "foley_plr_step",
            "foley_plr_step_unres",
            "foley_npc_step",
            "foley_dog_step",
            "item",
            "weapon_drone",
            "explosion1",
            "explosion2",
            "explosion3",
            "explosion4",
            "explosion5",
            "effects1",
            "effects2",
            "effects3",
            "effects2d1",
            "effects2d2",
            "norestrict",
            "norestrict2d",
            "aircraft",
            "vehicle2d",
            "weapon_dist",
            "weapon_mid",
            "weapon",
            "weapon2d",
            "nonshock",
            "nonshock2",
            "effects2dlim",
            "voice_dog",
            "music_emitter",
            "voice_dog_attack",
            "voice",
            "local",
            "local2",
            "local3",
            "ambient",
            "plr_weap_fire_2d",
            "plr_weap_mech_2d",
            "hurt",
            "player1",
            "player2",
            "music",
            "musicnopause",
            "mission",
            "missionfx",
            "announcer",
            "shellshock",
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
                    auto* flags = reinterpret_cast<game::SoundFlags*>(&asset->head[i].flags);
                    ImGui::Text("probability: %f", asset->head[i].probability);
                    ImGui::Text("looping: %i", flags->u.looping);
                    ImGui::Text("master: %i", flags->u.master);
                    ImGui::Text("slave: %i", flags->u.slave);
                    ImGui::Text("full_dry_level: %i", flags->u.full_dry_level);
                    ImGui::Text("no_wet_level: %i", flags->u.no_wet_level);
                    ImGui::Text("is_3d: %i", flags->u.is_3d);
                    ImGui::Text("channel: (%s) %i", sound_channels[flags->u.channel].data(), flags->u.channel);
                    ImGui::Text("type: %i", flags->u.type);
                    ImGui::Text("unk2: %i", flags->u.unk2);
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
