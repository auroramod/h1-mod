#include <std_include.hpp>

#ifdef DEBUG
#include "loader/component_loader.hpp"

#include "component/command.hpp"
#include "component/scheduler.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "gui.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/io.hpp>

#include <json.hpp>

#pragma region macros

#define ADD_FLOAT(__string__, __dvar__) \
		ADD_FLOAT_STEP(__string__, __dvar__, 0.01f); \

#define ADD_FLOAT_STEP(__string__, __dvar__, __step__ ) \
		if (__dvar__ != nullptr) ImGui::DragFloat(__string__, &__dvar__->current.value, __step__, __dvar__->domain.value.min, __dvar__->domain.value.max); \

#define ADD_COLOUR(__string__, __dvar__) \
		if (__dvar__ != nullptr) ImGui::ColorEdit3(__string__, __dvar__->current.vector); \

#define ADD_INT(__string__, __dvar__) \
		if (__dvar__ != nullptr) ImGui::DragInt(__string__, &__dvar__->current.integer, 1.0f, __dvar__->domain.integer.min, __dvar__->domain.integer.max); \

#define ADD_BOOL(__string__, __dvar__) \
		if (__dvar__ != nullptr) ImGui::Checkbox(__string__, &__dvar__->current.enabled);

#pragma endregion

namespace gui::vision_editor
{
	namespace
	{
#pragma region dvars
		static game::dvar_t* sm_sunEnable = nullptr;
		static game::dvar_t* sm_sunShadowScale = nullptr;
		static game::dvar_t* sm_spotLimit = nullptr;
		static game::dvar_t* sm_qualitySpotShadow = nullptr;
		static game::dvar_t* sm_usedSunCascadeCount = nullptr;
		static game::dvar_t* sm_sunSampleSizeNear = nullptr;
		static game::dvar_t* sm_sunFilterRadius = nullptr;
		static game::dvar_t* sm_spotFilterRadius = nullptr;
		static game::dvar_t* r_specularColorScale = nullptr;
		static game::dvar_t* r_diffuseColorScale = nullptr;
		static game::dvar_t* r_veil = nullptr;
		static game::dvar_t* r_veilStrength = nullptr;
		static game::dvar_t* r_veilBackgroundStrength = nullptr;
		static game::dvar_t* r_tonemap = nullptr;
		static game::dvar_t* r_tonemapAuto = nullptr;
		static game::dvar_t* r_tonemapBlend = nullptr;
		static game::dvar_t* r_tonemapLockAutoExposureAdjust = nullptr;
		static game::dvar_t* r_tonemapAutoExposureAdjust = nullptr;
		static game::dvar_t* r_tonemapExposure = nullptr;
		static game::dvar_t* r_tonemapExposureAdjust = nullptr;
		static game::dvar_t* r_tonemapMaxExposure = nullptr;
		static game::dvar_t* r_tonemapAdaptSpeed = nullptr;
		static game::dvar_t* r_tonemapDarkEv = nullptr;
		static game::dvar_t* r_tonemapMidEv = nullptr;
		static game::dvar_t* r_tonemapLightEv = nullptr;
		static game::dvar_t* r_tonemapDarkExposureAdjust = nullptr;
		static game::dvar_t* r_tonemapMidExposureAdjust = nullptr;
		static game::dvar_t* r_tonemapLightExposureAdjust = nullptr;
		static game::dvar_t* r_tonemapMinExposureAdjust = nullptr;
		static game::dvar_t* r_tonemapMaxExposureAdjust = nullptr;
		static game::dvar_t* r_tonemapWhite = nullptr;
		static game::dvar_t* r_tonemapShoulder = nullptr;
		static game::dvar_t* r_tonemapCrossover = nullptr;
		static game::dvar_t* r_tonemapToe = nullptr;
		static game::dvar_t* r_tonemapBlack = nullptr;
		static game::dvar_t* r_aoDiminish = nullptr;
		static game::dvar_t* r_ssao = nullptr;
		static game::dvar_t* r_ssaoStrength = nullptr;
		static game::dvar_t* r_ssaoPower = nullptr;
		static game::dvar_t* r_ssaoMinStrengthDepth = nullptr;
		static game::dvar_t* r_ssaoMaxStrengthDepth = nullptr;
		static game::dvar_t* r_ssaoWidth = nullptr;
		static game::dvar_t* r_ssaoGapFalloff = nullptr;
		static game::dvar_t* r_ssaoGradientFalloff = nullptr;
		static game::dvar_t* r_ssaoFadeDepth = nullptr;
		static game::dvar_t* r_ssaoRejectDepth = nullptr;
		static game::dvar_t* r_sky_fog_min_angle = nullptr;
		static game::dvar_t* r_sky_fog_max_angle = nullptr;
		static game::dvar_t* r_sky_fog_intensity = nullptr;

		static game::dvar_t* r_filmTweakBrightness = nullptr;
		static game::dvar_t* r_filmTweakContrast = nullptr;
		static game::dvar_t* r_filmTweakDarkTint = nullptr;
		static game::dvar_t* r_filmTweakDesaturation = nullptr;
		static game::dvar_t* r_filmTweakDesaturationDark = nullptr;
		static game::dvar_t* r_filmTweakEnable = nullptr;
		static game::dvar_t* r_filmTweakInvert = nullptr;
		static game::dvar_t* r_filmTweakLightTint = nullptr;
		static game::dvar_t* r_filmTweakMediumTint = nullptr;
		static game::dvar_t* r_primaryLightTweakDiffuseStrength = nullptr;
		static game::dvar_t* r_primaryLightTweakSpecularStrength = nullptr;
		static game::dvar_t* r_viewModelPrimaryLightTweakDiffuseStrength = nullptr;
		static game::dvar_t* r_viewModelPrimaryLightTweakSpecularStrength = nullptr;

		static game::dvar_t* r_aoUseTweaks = nullptr;
		static game::dvar_t* r_colorScaleUseTweaks = nullptr;
		static game::dvar_t* r_filmUseTweaks = nullptr;
		static game::dvar_t* r_primaryLightUseTweaks = nullptr;
		static game::dvar_t* r_skyFogUseTweaks = nullptr;
		static game::dvar_t* r_ssaoUseTweaks = nullptr;
		static game::dvar_t* r_tonemapUseTweaks = nullptr;
		static game::dvar_t* r_veilUseTweaks = nullptr;
		static game::dvar_t* r_viewModelPrimaryLightUseTweaks = nullptr;
		static game::dvar_t* sm_shadowUseTweaks = nullptr;

		static game::dvar_t* r_drawSun = nullptr;
		static game::dvar_t* r_lightGridNonCompressed = nullptr;
#pragma endregion

		void dump_tweaks()
		{
			nlohmann::json obj;

#define DUMP_BOOL(__string__, __dvar__) \
		if (__dvar__ != nullptr) obj[#__dvar__] = (__dvar__->current.enabled ? "1" : "0");

#define DUMP_FLOAT(__string__, __dvar__) \
		if (__dvar__ != nullptr) obj[#__dvar__] = std::to_string(__dvar__->current.value);

#define DUMP_INT(__string__, __dvar__) \
		if (__dvar__ != nullptr) obj[#__dvar__] = std::to_string(__dvar__->current.integer);

#define DUMP_COLOUR(__string__, __dvar__) \
		if (__dvar__ != nullptr) obj[#__dvar__] = std::to_string(__dvar__->current.vector[0]) + " " + \
                                               std::to_string(__dvar__->current.vector[1]) + " " + \
                                               std::to_string(__dvar__->current.vector[2]) + " " + \
                                               std::to_string(__dvar__->current.vector[3]);

			DUMP_BOOL("ao (screen-space)", r_ssaoUseTweaks);
			DUMP_BOOL("ao", r_aoUseTweaks);
			DUMP_BOOL("colour scale", r_colorScaleUseTweaks);
			DUMP_BOOL("film", r_filmUseTweaks);
			DUMP_BOOL("primary light (viewmodel)", r_viewModelPrimaryLightUseTweaks);
			DUMP_BOOL("primary light", r_primaryLightUseTweaks);
			DUMP_BOOL("shadows", sm_shadowUseTweaks);
			DUMP_BOOL("sky fog", r_skyFogUseTweaks);
			DUMP_BOOL("tonemapping", r_tonemapUseTweaks);
			DUMP_BOOL("veil", r_veilUseTweaks);

			DUMP_FLOAT("ao diminish", r_aoDiminish);
			DUMP_INT("ssao mode", r_ssao);
			DUMP_FLOAT("ssao strength", r_ssaoStrength);
			DUMP_FLOAT("ssao power", r_ssaoPower);
			DUMP_FLOAT("ssao min strength depth", r_ssaoMinStrengthDepth);
			DUMP_FLOAT("ssao max strength depth", r_ssaoMaxStrengthDepth);
			DUMP_FLOAT("ssao width", r_ssaoWidth);
			DUMP_FLOAT("ssao gap falloff", r_ssaoGapFalloff);
			DUMP_FLOAT("ssao gradient falloff", r_ssaoGradientFalloff);
			DUMP_FLOAT("ssao fade depth", r_ssaoFadeDepth);
			DUMP_FLOAT("ssao reject depth", r_ssaoRejectDepth);

			DUMP_FLOAT("specular colour scale", r_specularColorScale);
			DUMP_FLOAT("diffuse colour scale", r_diffuseColorScale);

			DUMP_BOOL("enable tweak", r_filmTweakEnable);
			DUMP_FLOAT("brightness", r_filmTweakBrightness);
			DUMP_FLOAT("contrast", r_filmTweakContrast);
			DUMP_BOOL("invert", r_filmTweakInvert);
			DUMP_COLOUR("light tint", r_filmTweakLightTint);
			DUMP_COLOUR("medium tint", r_filmTweakMediumTint);
			DUMP_COLOUR("dark tint", r_filmTweakDarkTint);
			DUMP_FLOAT("desaturation", r_filmTweakDesaturation);
			DUMP_FLOAT("desaturation dark", r_filmTweakDesaturationDark);

			DUMP_INT("mode", r_tonemap);
			DUMP_BOOL("blend between exposures", r_tonemapBlend);
			DUMP_BOOL("auto-exposure", r_tonemapAuto);
			DUMP_BOOL("lock auto-exposure adjust", r_tonemapLockAutoExposureAdjust);
			DUMP_FLOAT("auto-exposure adjust (0 = auto)", r_tonemapAutoExposureAdjust);
			DUMP_FLOAT("exposure", r_tonemapExposure);
			DUMP_FLOAT("exposure adjust", r_tonemapExposureAdjust);
			DUMP_FLOAT("max exposure", r_tonemapMaxExposure);
			DUMP_FLOAT("adapt speed", r_tonemapAdaptSpeed);
			DUMP_FLOAT("dark ev", r_tonemapDarkEv);
			DUMP_FLOAT("mid ev", r_tonemapMidEv);
			DUMP_FLOAT("light ev", r_tonemapLightEv);
			DUMP_FLOAT("dark exposure adjust", r_tonemapDarkExposureAdjust);
			DUMP_FLOAT("mid exposure adjust", r_tonemapMidExposureAdjust);
			DUMP_FLOAT("light exposure adjust", r_tonemapLightExposureAdjust);
			DUMP_FLOAT("black point", r_tonemapBlack);
			DUMP_FLOAT("white point", r_tonemapWhite);
			DUMP_FLOAT("shoulder control", r_tonemapShoulder);
			DUMP_FLOAT("crossover control", r_tonemapCrossover);
			DUMP_FLOAT("toe control", r_tonemapToe);

			DUMP_FLOAT("diffuse strength (vm)", r_viewModelPrimaryLightTweakDiffuseStrength);
			DUMP_FLOAT("specular strength (vm)", r_viewModelPrimaryLightTweakSpecularStrength);

			DUMP_FLOAT("diffuse strength", r_primaryLightTweakDiffuseStrength);
			DUMP_FLOAT("specular strength", r_primaryLightTweakSpecularStrength);

			DUMP_BOOL("enable sun", sm_sunEnable);
			DUMP_FLOAT("sun shadow scale", sm_sunShadowScale);
			DUMP_INT("spot limit", sm_spotLimit);
			DUMP_BOOL("quality spot shadow", sm_qualitySpotShadow);
			DUMP_INT("used sun cascade count", sm_usedSunCascadeCount);
			DUMP_FLOAT("sun sample size near", sm_sunSampleSizeNear);
			DUMP_FLOAT("sun filter radius", sm_sunFilterRadius);
			DUMP_FLOAT("spot filter radius", sm_spotFilterRadius);

			DUMP_FLOAT("min angle", r_sky_fog_min_angle);
			DUMP_FLOAT("max angle", r_sky_fog_max_angle);
			DUMP_FLOAT("intensity", r_sky_fog_intensity);

			DUMP_BOOL("enable veil", r_veil);
			DUMP_FLOAT("strength", r_veilStrength);
			DUMP_FLOAT("background strength", r_veilBackgroundStrength);

			DUMP_BOOL("", r_drawSun);
			DUMP_BOOL("", r_lightGridNonCompressed);

			auto timestamp = []() -> std::string
			{
				tm ltime{};
				char timestamp[MAX_PATH] = { 0 };
				const auto time = _time64(nullptr);

				_localtime64_s(&ltime, &time);
				strftime(timestamp, sizeof(timestamp) - 1, "%Y-%m-%d-%H-%M-%S", &ltime);

				return timestamp;
			}();

			static const auto* mapname = game::Dvar_FindVar("mapname");
			const auto path = std::format("h2m-mod/vision_export/{}_{}.json", (mapname ? mapname->current.string : "export"), timestamp);
			utils::io::write_file(path, obj.dump(4));

			gui::notification("Exported tweak values to:", utils::string::va("\"%s\"", path.data()));
		}

		void render_window()
		{
			static auto* enabled = &gui::enabled_menus["vision_editor"];

			ImGui::Begin("Tweak Toggles", enabled);
			ADD_BOOL("ao (screen-space)", r_ssaoUseTweaks);
			ADD_BOOL("ao", r_aoUseTweaks);
			ADD_BOOL("colour scale", r_colorScaleUseTweaks);
			ADD_BOOL("film", r_filmUseTweaks);
			ADD_BOOL("primary light (viewmodel)", r_viewModelPrimaryLightUseTweaks);
			ADD_BOOL("primary light", r_primaryLightUseTweaks);
			ADD_BOOL("shadows", sm_shadowUseTweaks);
			ADD_BOOL("sky fog", r_skyFogUseTweaks);
			ADD_BOOL("tonemapping", r_tonemapUseTweaks);
			ADD_BOOL("veil", r_veilUseTweaks);
			ADD_BOOL("draw sun", r_drawSun);
			ADD_BOOL("use old lightgrids (restart map)", r_lightGridNonCompressed);

			if (ImGui::Button("export dvar tweaks"))
			{
				dump_tweaks();
			}
			if (ImGui::Button("map restart"))
			{
				command::execute("map_restart");
			}

			ImGui::End();

			ImGui::SetNextWindowSizeConstraints(ImVec2(500, 500), ImVec2(1000, 1000));
			ImGui::Begin("Vision Editor");

			if (ImGui::CollapsingHeader("ambient occlusion"))
			{
				ADD_FLOAT("ao diminish", r_aoDiminish);
				ADD_INT("ssao mode", r_ssao);
				ADD_FLOAT("ssao strength", r_ssaoStrength);
				ADD_FLOAT("ssao power", r_ssaoPower);
				ADD_FLOAT("ssao min strength depth", r_ssaoMinStrengthDepth);
				ADD_FLOAT("ssao max strength depth", r_ssaoMaxStrengthDepth);
				ADD_FLOAT("ssao width", r_ssaoWidth);
				ADD_FLOAT("ssao gap falloff", r_ssaoGapFalloff);
				ADD_FLOAT("ssao gradient falloff", r_ssaoGradientFalloff);
				ADD_FLOAT("ssao fade depth", r_ssaoFadeDepth);
				ADD_FLOAT("ssao reject depth", r_ssaoRejectDepth);
			}

			if (ImGui::CollapsingHeader("colour scale"))
			{
				ADD_FLOAT("specular colour scale", r_specularColorScale);
				ADD_FLOAT("diffuse colour scale", r_diffuseColorScale);
			}

			if (ImGui::CollapsingHeader("filmtweaks"))
			{
				ADD_BOOL("enable tweak", r_filmTweakEnable);
				ADD_FLOAT("brightness", r_filmTweakBrightness);
				ADD_FLOAT("contrast", r_filmTweakContrast);
				ADD_BOOL("invert", r_filmTweakInvert);
				ADD_COLOUR("light tint", r_filmTweakLightTint);
				ADD_COLOUR("medium tint", r_filmTweakMediumTint);
				ADD_COLOUR("dark tint", r_filmTweakDarkTint);
				ADD_FLOAT("desaturation", r_filmTweakDesaturation);
				ADD_FLOAT("desaturation dark", r_filmTweakDesaturationDark);
			}

			if (ImGui::CollapsingHeader("HDR tonemapping"))
			{
				ADD_INT("mode", r_tonemap);
				ADD_BOOL("blend between exposures", r_tonemapBlend);
				ADD_BOOL("auto-exposure", r_tonemapAuto);
				ADD_BOOL("lock auto-exposure adjust", r_tonemapLockAutoExposureAdjust);
				ADD_FLOAT("auto-exposure adjust (0 = auto)", r_tonemapAutoExposureAdjust);
				ADD_FLOAT("exposure", r_tonemapExposure);
				ADD_FLOAT("exposure adjust", r_tonemapExposureAdjust);
				ADD_FLOAT("max exposure", r_tonemapMaxExposure);
				ADD_FLOAT("adapt speed", r_tonemapAdaptSpeed);
				ADD_FLOAT("dark ev", r_tonemapDarkEv);
				ADD_FLOAT("mid ev", r_tonemapMidEv);
				ADD_FLOAT("light ev", r_tonemapLightEv);
				ADD_FLOAT("dark exposure adjust", r_tonemapDarkExposureAdjust);
				ADD_FLOAT("mid exposure adjust", r_tonemapMidExposureAdjust);
				ADD_FLOAT("light exposure adjust", r_tonemapLightExposureAdjust);
				ADD_FLOAT_STEP("black point", r_tonemapBlack, 1);
				ADD_FLOAT_STEP("white point", r_tonemapWhite, 10);
				ADD_FLOAT("shoulder control", r_tonemapShoulder);
				ADD_FLOAT("crossover control", r_tonemapCrossover);
				ADD_FLOAT("toe control", r_tonemapToe);
			}

			if (ImGui::CollapsingHeader("primary light (viewmodel)"))
			{
				ADD_FLOAT("diffuse strength (vm)", r_viewModelPrimaryLightTweakDiffuseStrength);
				ADD_FLOAT("specular strength (vm)", r_viewModelPrimaryLightTweakSpecularStrength);
			}

			if (ImGui::CollapsingHeader("primary light"))
			{
				ADD_FLOAT("diffuse strength", r_primaryLightTweakDiffuseStrength);
				ADD_FLOAT("specular strength", r_primaryLightTweakSpecularStrength);
			}

			if (ImGui::CollapsingHeader("shadows"))
			{
				ADD_BOOL("enable sun", sm_sunEnable);
				ADD_FLOAT("sun shadow scale", sm_sunShadowScale);
				ADD_INT("spot limit", sm_spotLimit);
				ADD_BOOL("quality spot shadow", sm_qualitySpotShadow);
				ADD_INT("used sun cascade count", sm_usedSunCascadeCount);
				ADD_FLOAT("sun sample size near", sm_sunSampleSizeNear);
				ADD_FLOAT("sun filter radius", sm_sunFilterRadius);
				ADD_FLOAT("spot filter radius", sm_spotFilterRadius);
			}

			if (ImGui::CollapsingHeader("sky fog"))
			{
				ADD_FLOAT_STEP("min angle", r_sky_fog_min_angle, 1.0f);
				ADD_FLOAT_STEP("max angle", r_sky_fog_max_angle, 1.0f);
				ADD_FLOAT("intensity", r_sky_fog_intensity);
			}
		
			if (ImGui::CollapsingHeader("veiling luminance (HDR glow)"))
			{
				ADD_BOOL("enable veil", r_veil);
				ADD_FLOAT("strength", r_veilStrength);
				ADD_FLOAT("background strength", r_veilBackgroundStrength);
			}
			
			ImGui::End();
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi() || game::environment::is_sp())
			{
				return;
			}

			scheduler::once([]()
			{
				sm_sunEnable = game::Dvar_FindVar("sm_sunEnable");
				sm_sunShadowScale = game::Dvar_FindVar("sm_sunShadowScale");
				sm_spotLimit = game::Dvar_FindVar("sm_spotLimit");
				sm_qualitySpotShadow = game::Dvar_FindVar("sm_qualitySpotShadow");
				sm_usedSunCascadeCount = game::Dvar_FindVar("sm_usedSunCascadeCount");
				sm_sunSampleSizeNear = game::Dvar_FindVar("sm_sunSampleSizeNear");
				sm_sunFilterRadius = game::Dvar_FindVar("sm_sunFilterRadius");
				sm_spotFilterRadius = game::Dvar_FindVar("sm_spotFilterRadius");
				r_specularColorScale = game::Dvar_FindVar("r_specularColorScale");
				r_diffuseColorScale = game::Dvar_FindVar("r_diffuseColorScale");
				r_veil = game::Dvar_FindVar("r_veil");
				r_veilStrength = game::Dvar_FindVar("r_veilStrength");
				r_veilBackgroundStrength = game::Dvar_FindVar("r_veilBackgroundStrength");
				r_tonemap = game::Dvar_FindVar("r_tonemap");
				r_tonemapAuto = game::Dvar_FindVar("r_tonemapAuto");
				r_tonemapBlend = game::Dvar_FindVar("r_tonemapBlend");
				r_tonemapLockAutoExposureAdjust = game::Dvar_FindVar("r_tonemapLockAutoExposureAdjust");
				r_tonemapAutoExposureAdjust = game::Dvar_FindVar("r_tonemapAutoExposureAdjust");
				r_tonemapExposure = game::Dvar_FindVar("r_tonemapExposure");
				r_tonemapExposureAdjust = game::Dvar_FindVar("r_tonemapExposureAdjust");
				r_tonemapMaxExposure = game::Dvar_FindVar("r_tonemapMaxExposure");
				r_tonemapAdaptSpeed = game::Dvar_FindVar("r_tonemapAdaptSpeed");
				r_tonemapDarkEv = game::Dvar_FindVar("r_tonemapDarkEv");
				r_tonemapMidEv = game::Dvar_FindVar("r_tonemapMidEv");
				r_tonemapLightEv = game::Dvar_FindVar("r_tonemapLightEv");
				r_tonemapDarkExposureAdjust = game::Dvar_FindVar("r_tonemapDarkExposureAdjust");
				r_tonemapMidExposureAdjust = game::Dvar_FindVar("r_tonemapMidExposureAdjust");
				r_tonemapLightExposureAdjust = game::Dvar_FindVar("r_tonemapLightExposureAdjust");
				r_tonemapMinExposureAdjust = game::Dvar_FindVar("r_tonemapMinExposureAdjust");
				r_tonemapMaxExposureAdjust = game::Dvar_FindVar("r_tonemapMaxExposureAdjust");
				r_tonemapWhite = game::Dvar_FindVar("r_tonemapWhite");
				r_tonemapShoulder = game::Dvar_FindVar("r_tonemapShoulder");
				r_tonemapCrossover = game::Dvar_FindVar("r_tonemapCrossover");
				r_tonemapToe = game::Dvar_FindVar("r_tonemapToe");
				r_tonemapBlack = game::Dvar_FindVar("r_tonemapBlack");
				r_aoDiminish = game::Dvar_FindVar("r_aoDiminish");
				r_ssao = game::Dvar_FindVar("r_ssao");
				r_ssaoStrength = game::Dvar_FindVar("r_ssaoStrength");
				r_ssaoPower = game::Dvar_FindVar("r_ssaoPower");
				r_ssaoMinStrengthDepth = game::Dvar_FindVar("r_ssaoMinStrengthDepth");
				r_ssaoMaxStrengthDepth = game::Dvar_FindVar("r_ssaoMaxStrengthDepth");
				r_ssaoWidth = game::Dvar_FindVar("r_ssaoWidth");
				r_ssaoGapFalloff = game::Dvar_FindVar("r_ssaoGapFalloff");
				r_ssaoGradientFalloff = game::Dvar_FindVar("r_ssaoGradientFalloff");
				r_ssaoFadeDepth = game::Dvar_FindVar("r_ssaoFadeDepth");
				r_ssaoRejectDepth = game::Dvar_FindVar("r_ssaoRejectDepth");
				r_sky_fog_min_angle = game::Dvar_FindVar("r_sky_fog_min_angle");
				r_sky_fog_max_angle = game::Dvar_FindVar("r_sky_fog_max_angle");
				r_sky_fog_intensity = game::Dvar_FindVar("r_sky_fog_intensity");

				r_filmTweakBrightness = game::Dvar_FindVar("r_filmTweakBrightness");
				r_filmTweakContrast = game::Dvar_FindVar("r_filmTweakContrast");
				r_filmTweakDarkTint = game::Dvar_FindVar("r_filmTweakDarkTint");
				r_filmTweakDesaturation = game::Dvar_FindVar("r_filmTweakDesaturation");
				r_filmTweakDesaturationDark = game::Dvar_FindVar("r_filmTweakDesaturationDark");
				r_filmTweakEnable = game::Dvar_FindVar("r_filmTweakEnable");
				r_filmTweakInvert = game::Dvar_FindVar("r_filmTweakInvert");
				r_filmTweakLightTint = game::Dvar_FindVar("r_filmTweakLightTint");
				r_filmTweakMediumTint = game::Dvar_FindVar("r_filmTweakMediumTint");
				r_primaryLightTweakDiffuseStrength = game::Dvar_FindVar("r_primaryLightTweakDiffuseStrength");
				r_primaryLightTweakSpecularStrength = game::Dvar_FindVar("r_primaryLightTweakSpecularStrength");
				r_viewModelPrimaryLightTweakDiffuseStrength = game::Dvar_FindVar("r_viewModelPrimaryLightTweakDiffuseStrength");
				r_viewModelPrimaryLightTweakSpecularStrength = game::Dvar_FindVar("r_viewModelPrimaryLightTweakSpecularStrength");

				r_aoUseTweaks = game::Dvar_FindVar("r_aoUseTweaks");
				r_colorScaleUseTweaks = game::Dvar_FindVar("r_colorScaleUseTweaks");
				r_filmUseTweaks = game::Dvar_FindVar("r_filmUseTweaks");
				r_primaryLightUseTweaks = game::Dvar_FindVar("r_primaryLightUseTweaks");
				r_skyFogUseTweaks = game::Dvar_FindVar("r_skyFogUseTweaks");
				r_ssaoUseTweaks = game::Dvar_FindVar("r_ssaoUseTweaks");
				r_tonemapUseTweaks = game::Dvar_FindVar("r_tonemapUseTweaks");
				r_veilUseTweaks = game::Dvar_FindVar("r_veilUseTweaks");
				r_viewModelPrimaryLightUseTweaks = game::Dvar_FindVar("r_viewModelPrimaryLightUseTweaks");
				sm_shadowUseTweaks = game::Dvar_FindVar("sm_shadowUseTweaks");

				r_drawSun = game::Dvar_FindVar("r_drawSun");
				r_lightGridNonCompressed = game::Dvar_FindVar("r_lightGridNonCompressed");
			}, scheduler::main);

			gui::register_menu("vision_editor", "Vision Editor", render_window);
		}
	};
}

REGISTER_COMPONENT(gui::vision_editor::component)
#endif
