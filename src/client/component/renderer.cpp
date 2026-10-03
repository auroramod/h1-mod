#include <std_include.hpp>
#include "renderer.hpp"

#include "dvars.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

static utils::hook::detour r_init_draw_method_hook;
static utils::hook::detour r_update_front_end_dvar_options_hook;

static game::dvar_t* r_red_dot_brightness_scale;
static game::dvar_t* r_use_custom_red_dot_brightness;
static float tonemap_highlight_range = 16.f;

#ifdef _DEBUG
static game::dvar_t* r_drawLightOrigins;
static game::dvar_t* r_drawModelNames;
static game::dvar_t* r_drawDynEntInfo;
static game::dvar_t* r_drawFxInfo;

static game::dvar_t* r_playerDrawDebugDistance;

namespace
{
	enum model_draw_e : int
	{
		off,
		static_models,
		dynent_models,
		scene_models,
		all,
	};
}

static const char* model_draw_s[] =
{
	"off",
	"static models",
	"dynent dynents",
	"scene models",
	"all",
	nullptr
};

static void VectorSubtract(const float va[3], const float vb[3], float out[3])
{
	out[0] = va[0] - vb[0];
	out[1] = va[1] - vb[1];
	out[2] = va[2] - vb[2];
}

static float Vec3SqrDistance(const float v1[3], const float v2[3])
{
	float out[3];

	VectorSubtract(v2, v1, out);

	return (out[0] * out[0]) + (out[1] * out[1]) + (out[2] * out[2]);
}

static void draw_text(const char* name, game::vec3_t& origin, game::vec4_t& color)
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

static void debug_draw_light_origins()
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

static void debug_draw_model_names()
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

static void debug_draw_dynent_info()
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

static void debug_draw_fx_info()
{
	if (!r_drawFxInfo || r_drawFxInfo->current.enabled == 0)
	{
		return;
	}

	const auto fxSystem = game::Fx_GetSystem();
	if ((fxSystem->systemFlags & 0x3) != 0)
	{
		return;
	}

	auto player = *game::mp::playerState;
	float playerPosition[3]{ player->origin[0], player->origin[1], player->origin[2] };

	auto distance = r_playerDrawDebugDistance->current.integer;
	auto sqrDist = distance * static_cast<float>(distance);

	static float fxInfoColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

	game::FX_WaitEnterReadSystemLock(fxSystem->lock);

	for (auto i = fxSystem->firstActiveEffect; i != fxSystem->firstNewEffect; ++i)
	{
		auto effectHandle = fxSystem->allEffectHandles[i & 0x7FF];
		auto effect = (game::FxEffect*)((char*)fxSystem->effects + (unsigned int)(16 * effectHandle));

		if (!effect->def)
			continue;

		if (Vec3SqrDistance(playerPosition, effect->frameNow.origin) < static_cast<float>(sqrDist))
		{
			draw_text(effect->def->name, effect->frameNow.origin, fxInfoColor);
		}
	}

	game::FX_ExitReadSystemLock(fxSystem->lock);
}
#endif

static std::unordered_map<std::string, float> tonemap_highlight_range_overrides =
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

void renderer::post_unpack()
{
	if (game::environment::is_dedi())
	{
		return;
	}

	dvars::r_fullbright = dvars::register_int("r_fullbright", 0, 0, 3, game::DVAR_ARCHIVE, "Toggles rendering without lighting");

	if (game::environment::is_sp())
	{
		r_init_draw_method_hook.create(0x1405467E0, &r_init_draw_method_stub);
		r_update_front_end_dvar_options_hook.create(0x140583560, &r_update_front_end_dvar_options_stub);
	}
	else
	{
		utils::hook::call(0x1405DD584, r_init_draw_method_stub); // R_InitDrawMethod
		utils::hook::call(0x1405FE3A6, r_update_front_end_dvar_options_stub); // R_UpdateFrontEndDvarOptions
	}

	// use "saved" flags
	//dvars_component::override::register_enum("r_normalMap", game::DVAR_ARCHIVE);
	//dvars_component::override::register_enum("r_specularMap", game::DVAR_ARCHIVE);
	//dvars_component::override::register_enum("r_specOccMap", game::DVAR_ARCHIVE);

	if (game::environment::is_mp())
	{
		// Adjust red dot brightness
		utils::hook::jump(0x1400EFA25, get_tonemap_highlight_range_stub(), true);
		utils::hook::call(0x1402BCFC6, db_ready_inside_loadxassets_stub); // DB_LoadXAssets -> Sys_IsDatabaseReady

		r_red_dot_brightness_scale = dvars::register_float("r_redDotBrightnessScale",
			1.f, 0.1f, 5.f, game::DVAR_ARCHIVE, "Adjust red-dot reticle brightness");

		r_use_custom_red_dot_brightness = dvars::register_bool("r_useCustomRedDotBrightness",
			true, game::DVAR_ARCHIVE, "Use custom red-dot brightness values");
	}

	// patch r_preloadShaders crash at init
	if (game::environment::is_sp())
	{
		utils::hook::jump(0x1405CF1F1, utils::hook::assemble(r_preload_shaders_stub_sp), true);
	}
	else
	{
		utils::hook::jump(0x14063E0C7, utils::hook::assemble(r_preload_shaders_stub_mp), true);
	}
	dvars_component::override::register_bool("r_preloadShaders", false, game::DVAR_ARCHIVE);

#ifdef _DEBUG
	if (game::environment::is_mp())
	{
		r_drawLightOrigins = dvars::register_bool("r_drawLightOrigins", false, game::DVAR_CHEAT, "Draw comworld light origins");
		r_drawModelNames = dvars::register_enum("r_drawModelNames", model_draw_s, model_draw_e::off, game::DVAR_CHEAT, "Draw all model names");
		r_drawDynEntInfo = dvars::register_bool("r_drawDynEntInfo", false, game::DVAR_CHEAT, "Draw dynent info");
		r_drawFxInfo = dvars::register_bool("r_drawFxInfo", false, game::DVAR_CHEAT, "Draw fx info");

		r_playerDrawDebugDistance = dvars::register_int("r_drawDebugDistance", 1000, 0, 50000, game::DVAR_ARCHIVE, "r_draw debug functions draw distance relative to the player");

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
				debug_draw_fx_info();
			}
		}, scheduler::renderer);
	}
#endif

	// set genericMaterialData in R_AddDObjToScene -> R_GetGfxEntIndex (fixes alien glow material)
	utils::hook::far_jump<0x140000000>(SELECT_VALUE(0x1401C0C3F, 0x1400EC5FF), utils::hook::assemble(r_get_gfx_ent_index_stub));
}

int renderer::get_fullbright_technique()
{
	switch (dvars::r_fullbright->current.integer)
	{
	case 2:
		return game::TECHNIQUE_LIT;
	case 3:
		return game::TECHNIQUE_WIREFRAME_SOLID;
	default:
		return game::TECHNIQUE_UNLIT;
	}
}

void renderer::gfxdrawmethod()
{
	game::gfxDrawMethod->drawScene = game::GFX_DRAW_SCENE_STANDARD;
	game::gfxDrawMethod->baseTechType = dvars::r_fullbright->current.enabled ? get_fullbright_technique() : game::TECHNIQUE_LIT;
	game::gfxDrawMethod->emissiveTechType = dvars::r_fullbright->current.enabled ? get_fullbright_technique() : game::TECHNIQUE_EMISSIVE;
	game::gfxDrawMethod->forceTechType = dvars::r_fullbright->current.enabled ? get_fullbright_technique() : 242;
}

void renderer::r_init_draw_method_stub()
{
	gfxdrawmethod();
}

bool renderer::r_update_front_end_dvar_options_stub()
{
	if (dvars::r_fullbright->modified)
	{
		game::Dvar_ClearModified(dvars::r_fullbright);
		game::R_SyncRenderThread();

		gfxdrawmethod();
	}

	if (game::environment::is_sp())
	{
		return r_update_front_end_dvar_options_hook.invoke<bool>();
	}

	// R_UpdateFrontEndDvarOptions
	return utils::hook::invoke<bool>(0x1405FF9E0);
}

void renderer::set_tonemap_highlight_range()
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

int renderer::db_ready_inside_loadxassets_stub()
{
	set_tonemap_highlight_range();

	// Sys_IsDatabaseReady
	return utils::hook::invoke<int>(0x14042B0A0);
}

int renderer::get_red_dot_brightness()
{
	static auto* r_tonemap_highlight_range = game::Dvar_FindVar("r_tonemapHighlightRange");
	auto value = r_tonemap_highlight_range->current.value;
	if (r_use_custom_red_dot_brightness->current.enabled)
	{
		value = tonemap_highlight_range * r_red_dot_brightness_scale->current.value;
	}

	return *reinterpret_cast<int*>(&value);
}

void* renderer::get_tonemap_highlight_range_stub()
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

		a.mov(dword_ptr(r9, 0x1E5C), eax);

		a.jmp(0x1400EFA36);
	});
}

void renderer::r_preload_shaders_stub_sp(utils::hook::assembler& a)
{
	const auto is_zero = a.newLabel();

	a.mov(rax, qword_ptr(0x1523FFF30));
	a.test(rax, rax);
	a.jz(is_zero);

	a.mov(rcx, qword_ptr(rax, 0x540C68));
	a.jmp(0x1405CF1FF);

	a.bind(is_zero);
	a.jmp(0x1405CF20A);
}

void renderer::r_preload_shaders_stub_mp(utils::hook::assembler& a)
{
	const auto is_zero = a.newLabel();

	a.mov(rax, qword_ptr(0x1525C4ED0));
	a.test(rax, rax);
	a.jz(is_zero);

	a.lea(r9, qword_ptr(rsp, 0x38));
	a.mov(rcx, qword_ptr(rax, 0x540C68));
	a.jmp(0x14063E0DA);

	a.bind(is_zero);
	a.jmp(0x14063E0E5);
}

void renderer::r_get_gfx_ent_index_stub(utils::hook::assembler& a)
{
	a.movss(dword_ptr(rbx, 4), xmm6);
	a.mov(dword_ptr(rbx, 8), r9); // gfxEnt->genericMaterialData
	a.mov(dword_ptr(rbx, 0), esi);
	a.jmp(SELECT_VALUE(0x1401C0C46, 0x1400EC606));
}

REGISTER_COMPONENT(renderer)
