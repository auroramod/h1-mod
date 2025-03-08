#include <std_include.hpp>

#ifdef DEBUG
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "gui.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>

#pragma region macros

#define READ_BOOL(__var__, __dvar__) \
		if (__dvar__ != nullptr) __var__ = &__dvar__->current.enabled; \

#define READ_FLOAT(__var__, __dvar__) \
		if (__dvar__ != nullptr) __var__ = __dvar__->current.value; \

#define READ_VEC3(__var__, __dvar__) \
		if (__dvar__ != nullptr) \
		{ \
			for (auto i = 0; i < 3; i++) \
				__var__[i] = __dvar__->current.color[i]; \
		} \

#define WRITE_BOOL(__name__, __var__) \
		if (ImGui::Checkbox(__name__, &__var__)) build_atmos_fog_buffer(); \

#define WRITE_FLOAT(__name__, __var__, __step__, __dvar__) \
		if (__dvar__ != nullptr && ImGui::DragFloat(__name__, &__var__, __step__, __dvar__->domain.value.min, __dvar__->domain.value.max)) build_atmos_fog_buffer(); \

#define WRITE_VEC3(__name__, __var__, __step__, __dvar__) \
		if (__dvar__ != nullptr && ImGui::DragFloat3(__name__, __var__, __step__, __dvar__->domain.value.min, __dvar__->domain.value.max)) build_atmos_fog_buffer(); \

#define WRITE_COLOUR(__name__, __var__) \
		if (ImGui::ColorEdit3(__name__, __var__)) build_atmos_fog_buffer(); \

#define M_LN2 0.69314718f

#pragma endregion

namespace gui::fog_editor
{
	namespace
	{
		bool parse_art_file = true;

		utils::hook::detour cg_parse_client_visionset_triggers;
		void cg_parse_client_visionset_triggers_stub(const char* buffer)
		{
			return cg_parse_client_visionset_triggers.invoke<void>(parse_art_file ? buffer : nullptr);
		}

		bool atmos_height_fog = false;
		float atmos_height_base = 0.0f;
		float atmos_height_half_dist = 100000.0f;
		float atmos_start_dist = 0.0f;
		float atmos_half_dist = 5000.0f;
		float atmos_sun_fog_colour[3] = {};
		float atmos_haze_colour[3] = {};
		float atmos_haze_strength = 0.5f;
		float atmos_haze_spread = 0.25f;
		float atmos_extinction_strength = 1.0f;
		float atmos_inscatter_strength = 0.0f;
		float atmos_distance_scale = 1.0f;
		float atmos_sky_distance = 100000.0f;
		bool atmos_sky_angular_falloff = false;
		float atmos_sky_falloff_start = 0.0f;
		float atmos_sky_falloff_range = 90.0f;
		float atmos_sun_dir[3] = {};

		void build_atmos_fog_buffer()
		{
			auto buffer = utils::string::va(
				"%g %g %.0f %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g",
				atmos_start_dist,
				M_LN2 / atmos_half_dist,
				0.0f, // transition time
				atmos_sun_fog_colour[0],
				atmos_sun_fog_colour[1],
				atmos_sun_fog_colour[2],
				atmos_haze_colour[0],
				atmos_haze_colour[1],
				atmos_haze_colour[2],
				atmos_haze_strength,
				atmos_haze_spread,
				atmos_extinction_strength,
				atmos_inscatter_strength,
				atmos_distance_scale,
				atmos_sky_distance,
				atmos_sky_angular_falloff ? atmos_sky_falloff_start : -90.0f,
				atmos_sky_angular_falloff ? atmos_sky_falloff_range : 9000.0f,
				atmos_sun_dir[0],
				atmos_sun_dir[1],
				atmos_sun_dir[2],
				atmos_height_fog ? atmos_height_base : 0.0f,
				M_LN2 / (atmos_height_fog ? atmos_height_half_dist : 100000.0f));
			game::G_SetFog(buffer, 0, 1);
		}

		void render_window()
		{
			static auto* enabled = &gui::enabled_menus["fog_editor"];

#pragma region dvars
			static auto* g_atmosFogStartDistanceReadOnly = game::Dvar_FindVar("g_atmosFogStartDistanceReadOnly");
			static auto* g_atmosFogHalfPlaneDistanceReadOnly = game::Dvar_FindVar("g_atmosFogHalfPlaneDistanceReadOnly");
			static auto* g_atmosFogSunFogColorReadOnly = game::Dvar_FindVar("g_atmosFogSunFogColorReadOnly");
			static auto* g_atmosFogHazeColorReadOnly = game::Dvar_FindVar("g_atmosFogHazeColorReadOnly");
			static auto* g_atmosFogHazeStrengthReadOnly = game::Dvar_FindVar("g_atmosFogHazeStrengthReadOnly");
			static auto* g_atmosFogHazeSpreadReadOnly = game::Dvar_FindVar("g_atmosFogHazeSpreadReadOnly");
			static auto* g_atmosFogExtinctionStrengthReadOnly = game::Dvar_FindVar("g_atmosFogExtinctionStrengthReadOnly");
			static auto* g_atmosFogInScatterStrengthReadOnly = game::Dvar_FindVar("g_atmosFogInScatterStrengthReadOnly");
			static auto* g_atmosFogDistanceScaleReadOnly = game::Dvar_FindVar("g_atmosFogDistanceScaleReadOnly");
			static auto* g_atmosFogSkyDistanceReadOnly = game::Dvar_FindVar("g_atmosFogSkyDistanceReadOnly");
			static auto* g_atmosFogSunDirectionReadOnly = game::Dvar_FindVar("g_atmosFogSunDirectionReadOnly");
			static auto* g_atmosFogHeightFogEnabledReadOnly = game::Dvar_FindVar("g_atmosFogHeightFogEnabledReadOnly");
			static auto* g_atmosFogSkyAngularFalloffEnabledReadOnly = game::Dvar_FindVar("g_atmosFogSkyAngularFalloffEnabledReadOnly");
			static auto* g_atmosFogSkyFalloffStartAngleReadOnly = game::Dvar_FindVar("g_atmosFogSkyFalloffStartAngleReadOnly");
			static auto* g_atmosFogSkyFalloffAngleRangeReadOnly = game::Dvar_FindVar("g_atmosFogSkyFalloffAngleRangeReadOnly");
			static auto* g_atmosFogHeightFogBaseHeightReadOnly = game::Dvar_FindVar("g_atmosFogHeightFogBaseHeightReadOnly");
			static auto* g_atmosFogHeightFogHalfPlaneDistanceReadOnly = game::Dvar_FindVar("g_atmosFogHeightFogHalfPlaneDistanceReadOnly");
#pragma endregion

			ImGui::SetNextWindowSizeConstraints(ImVec2(500, 500), ImVec2(1000, 1000));
			ImGui::Begin("Fog Editor");

			if (ImGui::Button("read dvars"))
			{
				READ_BOOL(atmos_height_fog, g_atmosFogHeightFogEnabledReadOnly);
				READ_FLOAT(atmos_height_base, g_atmosFogHeightFogBaseHeightReadOnly);
				READ_FLOAT(atmos_height_half_dist, g_atmosFogHeightFogHalfPlaneDistanceReadOnly);
				READ_FLOAT(atmos_start_dist, g_atmosFogStartDistanceReadOnly);
				READ_FLOAT(atmos_half_dist, g_atmosFogHalfPlaneDistanceReadOnly);
				READ_VEC3(atmos_sun_fog_colour, g_atmosFogSunFogColorReadOnly);
				READ_VEC3(atmos_haze_colour, g_atmosFogHazeColorReadOnly);
				READ_FLOAT(atmos_haze_strength, g_atmosFogHazeStrengthReadOnly);
				READ_FLOAT(atmos_haze_spread, g_atmosFogHazeSpreadReadOnly);
				READ_FLOAT(atmos_extinction_strength, g_atmosFogExtinctionStrengthReadOnly);
				READ_FLOAT(atmos_inscatter_strength, g_atmosFogInScatterStrengthReadOnly);
				READ_FLOAT(atmos_distance_scale, g_atmosFogDistanceScaleReadOnly);
				READ_FLOAT(atmos_sky_distance, g_atmosFogSkyDistanceReadOnly);
				READ_FLOAT(atmos_sky_angular_falloff, g_atmosFogSkyAngularFalloffEnabledReadOnly);
				READ_FLOAT(atmos_sky_falloff_start, g_atmosFogSkyFalloffStartAngleReadOnly);
				READ_FLOAT(atmos_sky_falloff_range, g_atmosFogSkyFalloffAngleRangeReadOnly);
				READ_VEC3(atmos_sun_dir, g_atmosFogSunDirectionReadOnly);
			}

			ImGui::Checkbox("parse fog file on map load", &parse_art_file);

			WRITE_BOOL("enable height fog", atmos_height_fog);
			WRITE_FLOAT("height base", atmos_height_base, 10.0f, g_atmosFogHeightFogBaseHeightReadOnly);
			WRITE_FLOAT("height half dist", atmos_height_half_dist, 10.0f, g_atmosFogHeightFogHalfPlaneDistanceReadOnly);
			WRITE_FLOAT("start dist", atmos_start_dist, 10.0f, g_atmosFogStartDistanceReadOnly);
			WRITE_FLOAT("half dist", atmos_half_dist, 10.0f, g_atmosFogHalfPlaneDistanceReadOnly);
			WRITE_COLOUR("sun fog colour", atmos_sun_fog_colour);
			WRITE_COLOUR("haze colour", atmos_haze_colour);
			WRITE_FLOAT("haze strength", atmos_haze_strength, 0.01f, g_atmosFogHazeStrengthReadOnly);
			WRITE_FLOAT("haze spread", atmos_haze_spread, 0.01f, g_atmosFogHazeSpreadReadOnly);
			WRITE_FLOAT("extinction strength", atmos_extinction_strength, 0.01f, g_atmosFogExtinctionStrengthReadOnly);
			WRITE_FLOAT("inscatter strength", atmos_inscatter_strength, 0.1f, g_atmosFogInScatterStrengthReadOnly);
			WRITE_FLOAT("distance scale", atmos_distance_scale, 0.1f, g_atmosFogDistanceScaleReadOnly);
			WRITE_FLOAT("sky distance", atmos_sky_distance, 100.0f, g_atmosFogSkyDistanceReadOnly);
			WRITE_BOOL("sky angular falloff", atmos_sky_angular_falloff);
			WRITE_FLOAT("sky falloff start", atmos_sky_falloff_start, 1.0f, g_atmosFogSkyFalloffStartAngleReadOnly);
			WRITE_FLOAT("sky falloff range", atmos_sky_falloff_range, 10.0f, g_atmosFogSkyFalloffAngleRangeReadOnly);
			WRITE_VEC3("sun direction", atmos_sun_dir, 0.01f, g_atmosFogSunDirectionReadOnly);

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

			cg_parse_client_visionset_triggers.create(0x10F920_b, &cg_parse_client_visionset_triggers_stub);
			gui::register_menu("fog_editor", "Fog Editor", render_window);
		}
	};
}

REGISTER_COMPONENT(gui::fog_editor::component)
#endif
