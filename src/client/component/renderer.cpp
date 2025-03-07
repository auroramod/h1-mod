#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "dvars.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace renderer
{
	namespace
	{
		utils::hook::detour r_init_draw_method_hook;
		utils::hook::detour r_update_front_end_dvar_options_hook;

		utils::hook::detour db_load_xassets_hook;

		game::dvar_t* r_red_dot_brightness_scale;
		game::dvar_t* r_use_custom_red_dot_brightness;
		float tonemap_highlight_range = 16.f;

#ifdef DEBUG
		game::dvar_t* r_drawLightOrigins;
		game::dvar_t* r_drawModelNames;
		game::dvar_t* r_drawDynEntInfo;
		game::dvar_t* r_playerDrawDebugDistance;

		enum model_draw_e : int
		{
			off,
			static_models,
			dynent_models,
			scene_models,
			all,
		};

		static const char* model_draw_s[] =
		{
			"off",
			"static models",
			"dynent dynents",
			"scene models",
			"all",
			nullptr
		};
#endif

		std::unordered_map<std::string, float> tonemap_highlight_range_overrides =
		{
			// all these are 16 by default (except mp_bog & mp_cargoship which have it at 0) which makes red dots hard to see
			{"mp_convoy", 22.f},
			{"mp_backlot", 20.f},
			{"mp_bog", 12.f},
			{"mp_bloc", 30.f},
			{"mp_countdown", 20.f},
			{"mp_crash", 20.f},
			{"mp_creek", 20.f},
			{"mp_crossfire", 26.f},
			{"mp_citystreets", 30.f},
			{"mp_farm", 20.f},
			{"mp_overgrown", 22.f},
			{"mp_pipeline", 26.f},
			{"mp_shipment", 20.f},
			{"mp_showdown", 24.f},
			{"mp_strike", 24.f},
			{"mp_vacant", 20.f},
			{"mp_cargoship", 14.f},
			{"mp_crash_snow", 24.f},
			{"mp_bog_summer", 24.f},
		};

		int get_fullbright_technique()
		{
			switch (dvars::r_fullbright->current.integer)
			{
			case 2:
				return 13;
			default:
				return game::TECHNIQUE_UNLIT;
			}
		}

		void gfxdrawmethod()
		{
			game::gfxDrawMethod->drawScene = game::GFX_DRAW_SCENE_STANDARD;
			game::gfxDrawMethod->baseTechType = dvars::r_fullbright->current.enabled ? get_fullbright_technique() : game::TECHNIQUE_LIT;
			game::gfxDrawMethod->emissiveTechType = dvars::r_fullbright->current.enabled ? get_fullbright_technique() : game::TECHNIQUE_EMISSIVE;
			game::gfxDrawMethod->forceTechType = dvars::r_fullbright->current.enabled ? get_fullbright_technique() : 242;
		}

		void r_init_draw_method_stub()
		{
			gfxdrawmethod();
		}

		bool r_update_front_end_dvar_options_stub()
		{
			if (dvars::r_fullbright->modified)
			{
				game::Dvar_ClearModified(dvars::r_fullbright);
				game::R_SyncRenderThread();

				gfxdrawmethod();
			}

			return r_update_front_end_dvar_options_hook.invoke<bool>();
		}

		void set_tonemap_highlight_range()
		{
			auto* mapname = game::Dvar_FindVar("mapname");
			if (mapname != nullptr && tonemap_highlight_range_overrides.find(mapname->current.string)
				!= tonemap_highlight_range_overrides.end())
			{
				tonemap_highlight_range = tonemap_highlight_range_overrides[mapname->current.string];
			}
			else
			{
				tonemap_highlight_range = 16.f;
			}
		}

		void db_load_xassets_stub(void* a1, void* a2, void* a3)
		{
			set_tonemap_highlight_range();
			db_load_xassets_hook.invoke<void>(a1, a2, a3);
		}

		int get_red_dot_brightness()
		{
			static auto* r_tonemap_highlight_range = game::Dvar_FindVar("r_tonemapHighlightRange");
			auto value = r_tonemap_highlight_range->current.value;
			if (r_use_custom_red_dot_brightness->current.enabled)
			{
				value = tonemap_highlight_range * r_red_dot_brightness_scale->current.value;
			}

			return *reinterpret_cast<int*>(&value);
		}

		void* get_tonemap_highlight_range_stub()
		{
			return utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.push(r9);
				a.push(rax);
				a.pushad64();
				a.call_aligned(get_red_dot_brightness);
				a.mov(qword_ptr(rsp, 0x80), rax);
				a.popad64();
				a.pop(rax);
				a.pop(r9);

				a.mov(dword_ptr(r9, 0x1E84), eax);

				a.jmp(0x1C4136_b);
			});
		}

		void r_preload_shaders_stub(utils::hook::assembler& a)
		{
			const auto is_zero = a.newLabel();

			a.mov(rax, qword_ptr(SELECT_VALUE(0x123FFF30_b, 0x111DC230_b)));
			a.test(rax, rax);
			a.jz(is_zero);

			a.mov(rcx, qword_ptr(rax, 0x540C68));
			a.jmp(SELECT_VALUE(0x5CF1FF_b, 0x6E76FF_b));

			a.bind(is_zero);
			a.jmp(SELECT_VALUE(0x5CF20A_b, 0x6E7722_b));
		}

#ifdef DEBUG
		void VectorSubtract(const float va[3], const float vb[3], float out[3])
		{
			out[0] = va[0] - vb[0];
			out[1] = va[1] - vb[1];
			out[2] = va[2] - vb[2];
		}

		float Vec3SqrDistance(const float v1[3], const float v2[3])
		{
			float out[3];

			VectorSubtract(v2, v1, out);

			return (out[0] * out[0]) + (out[1] * out[1]) + (out[2] * out[2]);
		}

		inline void draw_text(const char* name, game::vec3_t& origin, game::vec4_t& color)
		{
			game::vec2_t screen{};
			if (game::CG_WorldPosToScreenPosReal(0, game::ScrPlace_GetActivePlacement(), origin, screen))
			{
				const auto font = game::R_RegisterFont("fonts/fira_mono_regular.ttf", 25);
				if (font)
				{
					game::R_AddCmdDrawText(name, 0x7FFFFFFF, font, screen[0], screen[1], 1.f, 1.f, 0.0f, color, 6);
				}
			}
		}

		void debug_draw_light_origins()
		{
			if (!r_drawLightOrigins || !r_drawLightOrigins->current.enabled)
			{
				return;
			}

			auto player = *game::mp::playerState;
			float playerPosition[3]{ player->origin[0], player->origin[1], player->origin[2] };

			auto mapname = game::Dvar_FindVar("mapname");
			auto comWorld = game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_COMWORLD, utils::string::va("maps/mp/%s.d3dbsp", mapname->current.string), 0).comWorld;
			if (comWorld == nullptr)
			{
				comWorld = game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_COMWORLD, utils::string::va("maps/%s.d3dbsp", mapname->current.string), 0).comWorld;
				if (comWorld == nullptr)
				{
					return;
				}
			}

			auto distance = r_playerDrawDebugDistance->current.integer;
			auto sqrDist = distance * static_cast<float>(distance);

			float textColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
			auto scene = *game::scene;

			for (size_t i = 0; i < comWorld->primaryLightCount; i++)
			{
				auto light = comWorld->primaryLights[i];
				const auto dist = Vec3SqrDistance(playerPosition, light.origin);
				if (dist < static_cast<float>(sqrDist))
				{
					const auto text = utils::string::va("%f, %f, %f (%d)", light.origin[0], light.origin[1], light.origin[2], i);
					draw_text(text, light.origin, textColor);
				}
			}
		}

		void debug_draw_model_names()
		{
			if (!r_drawModelNames || r_drawModelNames->current.integer == model_draw_e::off)
			{
				return;
			}

			auto player = *game::mp::playerState;
			float playerPosition[3]{ player->origin[0], player->origin[1], player->origin[2] };

			auto mapname = game::Dvar_FindVar("mapname");
			auto gfxAsset = game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_GFXWORLD, utils::string::va("maps/mp/%s.d3dbsp", mapname->current.string), 0).gfxWorld;
			if (gfxAsset == nullptr)
			{
				gfxAsset = game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_GFXWORLD, utils::string::va("maps/%s.d3dbsp", mapname->current.string), 0).gfxWorld;
				if (gfxAsset == nullptr)
				{
					return;
				}
			}

			auto clipMap = game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_CLIPMAP, utils::string::va("maps/mp/%s.d3dbsp", mapname->current.string), 0).clipMap;
			if (clipMap == nullptr)
			{
				clipMap = game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_CLIPMAP, utils::string::va("maps/%s.d3dbsp", mapname->current.string), 0).clipMap;
				if (clipMap == nullptr)
				{
					return;
				}
			}

			auto distance = r_playerDrawDebugDistance->current.integer;
			auto sqrDist = distance * static_cast<float>(distance);

			static float staticModelsColor[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
			static float dynEntModelsColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
			static float sceneModelsColor[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
			static float dobjsColor[4] = { 0.0f, 1.0f, 1.0f, 1.0f };
			auto scene = *game::scene;

			switch (r_drawModelNames->current.integer)
			{
			case model_draw_e::all:
			case model_draw_e::static_models:
				for (unsigned int i = 0; i < gfxAsset->dpvs.smodelCount; i++)
				{
					auto staticModel = gfxAsset->dpvs.smodelDrawInsts[i];
					if (!staticModel.model)
						continue;

					if (Vec3SqrDistance(playerPosition, staticModel.placement.origin) < static_cast<float>(sqrDist))
					{
						draw_text(staticModel.model->name, staticModel.placement.origin, staticModelsColor);
					}
				}
				if (r_drawModelNames->current.integer != model_draw_e::all) break;
			case model_draw_e::dynent_models:
				for (unsigned short i = 0; i < clipMap->dynEntCount[0]; i++)
				{
					auto* dynent_client = &clipMap->dynEntClientList[0][i];
					auto* dynent_pose = &clipMap->dynEntPoseList[0][i];

					if (!dynent_client || !dynent_pose || !dynent_client->activeModel)
						continue;

					if (Vec3SqrDistance(playerPosition, dynent_pose->pose.origin) < static_cast<float>(sqrDist))
					{
						draw_text(dynent_client->activeModel->name, dynent_pose->pose.origin, dynEntModelsColor);
					}
				}
				if (r_drawModelNames->current.integer != model_draw_e::all) break;
			case model_draw_e::scene_models:
				for (int i = 0; i < scene.sceneModelCount; i++)
				{
					if (!scene.sceneModel[i].model)
						continue;

					if (Vec3SqrDistance(playerPosition, scene.sceneModel[i].placement.base.origin) < static_cast<float>(sqrDist))
					{
						draw_text(scene.sceneModel[i].model->name, scene.sceneModel[i].placement.base.origin, sceneModelsColor);
					}
				}
				break;
			default:
				break;
			}
		}

		void debug_draw_dynent_info()
		{
			if (!r_drawDynEntInfo || r_drawDynEntInfo->current.enabled == 0)
			{
				return;
			}

			auto player = *game::mp::playerState;
			float playerPosition[3]{ player->origin[0], player->origin[1], player->origin[2] };

			auto mapname = game::Dvar_FindVar("mapname");

			auto clipMap = game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_CLIPMAP, utils::string::va("maps/mp/%s.d3dbsp", mapname->current.string), 0).clipMap;
			if (clipMap == nullptr)
			{
				clipMap = game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_CLIPMAP, utils::string::va("maps/%s.d3dbsp", mapname->current.string), 0).clipMap;
				if (clipMap == nullptr)
				{
					return;
				}
			}

			auto distance = r_playerDrawDebugDistance->current.integer;
			auto sqrDist = distance * static_cast<float>(distance);

			static float dynEntInfoColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

			for (auto i = 0; i < clipMap->dynEntCount[0]; i++)
			{
				auto* dynent_def = &clipMap->dynEntDefList[0][i];
				auto* dynent_pose = &clipMap->dynEntPoseList[0][i];

				if (!dynent_def || !dynent_pose)
					continue;

				if (Vec3SqrDistance(playerPosition, dynent_pose->pose.origin) < static_cast<float>(sqrDist))
				{
					const auto text = utils::string::va("model: ^1%s^7\ndestroy fx: ^2%s^7\nsound: ^3%s^7\nphys preset: ^4%s^7\n",
						dynent_def->baseModel ? dynent_def->baseModel->name : "",
						dynent_def->destroyFx ? dynent_def->destroyFx->name : "",
						dynent_def->sound ? dynent_def->sound->name : "",
						dynent_def->physPreset ? dynent_def->physPreset->name : "");
					draw_text(text, dynent_pose->pose.origin, dynEntInfoColor);
				}
			}
		}
#endif
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			dvars::r_fullbright = dvars::register_int("r_fullbright", 0, 0, 2, game::DVAR_FLAG_SAVED, "Toggles rendering without lighting");

			r_init_draw_method_hook.create(SELECT_VALUE(0x5467E0_b, 0x669580_b), &r_init_draw_method_stub);
			r_update_front_end_dvar_options_hook.create(SELECT_VALUE(0x583560_b, 0x6A78C0_b), &r_update_front_end_dvar_options_stub);

			// use "saved" flags
			dvars::override::register_enum("r_normalMap", game::DVAR_FLAG_SAVED);
			dvars::override::register_enum("r_specularMap", game::DVAR_FLAG_SAVED);
			dvars::override::register_enum("r_specOccMap", game::DVAR_FLAG_SAVED);

			if (game::environment::is_mp())
			{
				// Adjust red dot brightness
				utils::hook::jump(0x1C4125_b, get_tonemap_highlight_range_stub(), true);
				db_load_xassets_hook.create(0x397500_b, db_load_xassets_stub);

				r_red_dot_brightness_scale = dvars::register_float("r_redDotBrightnessScale", 
					1.f, 0.1f, 5.f, game::DVAR_FLAG_SAVED, "Adjust red-dot reticle brightness");

				r_use_custom_red_dot_brightness = dvars::register_bool("r_useCustomRedDotBrightness",
					true, game::DVAR_FLAG_SAVED, "Use custom red-dot brightness values");
			}

			// patch r_preloadShaders crash at init
			utils::hook::jump(SELECT_VALUE(0x5CF1F1_b, 0x6E76F1_b), utils::hook::assemble(r_preload_shaders_stub), true);
			dvars::override::register_bool("r_preloadShaders", false, game::DVAR_FLAG_SAVED);

#ifdef DEBUG
			if (!game::environment::is_mp())
			{
				return;
			}

			r_drawLightOrigins = dvars::register_bool("r_drawLightOrigins", false, game::DVAR_FLAG_CHEAT, "Draw comworld light origins");
			r_drawModelNames = dvars::register_enum("r_drawModelNames", model_draw_s, model_draw_e::off, game::DVAR_FLAG_CHEAT, "Draw all model names");
			r_drawDynEntInfo = dvars::register_bool("r_drawDynEntInfo", false, game::DVAR_FLAG_CHEAT, "Draw dynent info");
			r_playerDrawDebugDistance = dvars::register_int("r_drawDebugDistance", 1000, 0, 50000, game::DVAR_FLAG_SAVED, "r_draw debug functions draw distance relative to the player");

			scheduler::loop([]
			{
				static const auto* in_firing_range = game::Dvar_FindVar("virtualLobbyInFiringRange");
				if (!in_firing_range)
				{
					return;
				}

				if ( game::CL_IsCgameInitialized() || (game::VirtualLobby_Loaded() && in_firing_range->current.enabled) )
				{
					debug_draw_light_origins();
					debug_draw_model_names();
					debug_draw_dynent_info();
				}
			}, scheduler::renderer);
#endif
		}
	};
}

REGISTER_COMPONENT(renderer::component)
