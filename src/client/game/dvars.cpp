#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include <utils/string.hpp>

#include "game.hpp"
#include "dvars.hpp"
#include <component/console.hpp>
#include <utils/hook.hpp>

namespace dvars
{
	game::dvar_t* aimassist_enabled = nullptr;

	game::dvar_t* con_inputBoxColor = nullptr;
	game::dvar_t* con_inputHintBoxColor = nullptr;
	game::dvar_t* con_outputBarColor = nullptr;
	game::dvar_t* con_outputSliderColor = nullptr;
	game::dvar_t* con_outputWindowColor = nullptr;
	game::dvar_t* con_inputDvarMatchColor = nullptr;
	game::dvar_t* con_inputDvarValueColor = nullptr;
	game::dvar_t* con_inputDvarInactiveValueColor = nullptr;
	game::dvar_t* con_inputCmdMatchColor = nullptr;
	game::dvar_t* g_playerEjection = nullptr;
	game::dvar_t* g_playerCollision = nullptr;

	game::dvar_t* jump_enableFallDamage;

	game::dvar_t* r_fullbright;
	game::dvar_t* r_chams;

	game::dvar_t* g_gravity = nullptr;

	game::dvar_t* pm_bouncing = nullptr;

	game::dvar_t* cg_legacyCrashHandling;

	std::string dvar_get_vector_domain(const int components, const game::dvar_limits& domain)
	{
		if (domain.vector.min == -FLT_MAX)
		{
			if (domain.vector.max == FLT_MAX)
			{
				return utils::string::va("Domain is any %iD vector", components);
			}
			else
			{
				return utils::string::va("Domain is any %iD vector with components %g or smaller", components,
					domain.vector.max);
			}
		}
		else if (domain.vector.max == FLT_MAX)
		{
			return utils::string::va("Domain is any %iD vector with components %g or bigger", components,
				domain.vector.min);
		}
		else
		{
			return utils::string::va("Domain is any %iD vector with components from %g to %g", components,
				domain.vector.min, domain.vector.max);
		}
	}

	std::string dvar_get_domain(const game::dvar_type type, const game::dvar_limits& domain)
	{
		std::string str;

		switch (type)
		{
		case game::dvar_type::boolean:
			return "Domain is 0 or 1"s;

		case game::dvar_type::value:
			if (domain.value.min == -FLT_MAX)
			{
				if (domain.value.max == FLT_MAX)
				{
					return "Domain is any number"s;
				}
				else
				{
					return utils::string::va("Domain is any number %g or smaller", domain.value.max);
				}
			}
			else if (domain.value.max == FLT_MAX)
			{
				return utils::string::va("Domain is any number %g or bigger", domain.value.min);
			}
			else
			{
				return utils::string::va("Domain is any number from %g to %g", domain.value.min, domain.value.max);
			}

		case game::dvar_type::vec2:
			return dvar_get_vector_domain(2, domain);

		case game::dvar_type::rgb:
		case game::dvar_type::vec3:
			return dvar_get_vector_domain(3, domain);

		case game::dvar_type::vec4:
			return dvar_get_vector_domain(4, domain);

		case game::dvar_type::integer:
			if (domain.enumeration.stringCount == INT_MIN)
			{
				if (domain.integer.max == INT_MAX)
				{
					return "Domain is any integer"s;
				}
				else
				{
					return utils::string::va("Domain is any integer %i or smaller", domain.integer.max);
				}
			}
			else if (domain.integer.max == INT_MAX)
			{
				return utils::string::va("Domain is any integer %i or bigger", domain.integer.min);
			}
			else
			{
				return utils::string::va("Domain is any integer from %i to %i", domain.integer.min, domain.integer.max);
			}

		case game::dvar_type::color:
			return "Domain is any 4-component color, in RGBA format"s;

		case game::dvar_type::enumeration:
			str = "Domain is one of the following:"s;

			for (auto string_index = 0; string_index < domain.enumeration.stringCount; ++string_index)
			{
				str += utils::string::va("\n  %2i: %s", string_index, domain.enumeration.strings[string_index]);
			}

			return str;

		case game::dvar_type::string:
			return "Domain is any text"s;

		default:
			return utils::string::va("unhandled dvar type '%i'", type);
		}
	}

	namespace
	{
		template <typename T>
		T* find_dvar(std::unordered_map<std::string, T>& map, const std::string& name)
		{
			auto i = map.find(name);
			if (i != map.end())
			{
				return &i->second;
			}

			return nullptr;
		}

		bool find_dvar(std::unordered_set<std::string>& set, const std::string& name)
		{
			return set.find(name) != set.end();
		}
	}

	constexpr int generate_hash(const char* string)
	{
		const char* v1;
		char v2, v6;
		int v4, v5, v7;
		char* end_ptr;

		v1 = string;
		v2 = *string;

		if (v2 == 48 && v1[1] == 120)
		{
			return strtoul(v1 + 2, &end_ptr, 16);
		}

		v4 = v2;

		if ((v2 - 65) <= 0x19u)
		{
			v4 = v2 + 32;
		}

		v5 = 0xB3CB2E29 * static_cast<unsigned int>(v4 ^ 0x319712C3);

		if (v2)
		{
			do
			{
				v6 = *++v1;
				v7 = v6;
				if ((v6 - 65) <= 0x19u)
				{
					v7 = v6 + 32;
				}

				v5 = 0xB3CB2E29 * static_cast<unsigned int>(v5 ^ v7);
			} 
			while (v6);
		}

		return v5;
	}

	std::vector<dvar_info> dvar_list =
	{
		{
			"accessToSubscriberContent",
			"Whether to display the subscriber maps.",
			generate_hash("accessToSubscriberContent")
		},
		{
			"aci",
			"",
			generate_hash("aci")
		},
		{
			"actionSlotsHide",
			"Hide the actionslots.",
			generate_hash("actionSlotsHide")
		},
		{
			"ai_grenadeReturn_approachMinDot",
			"Minimal dot product between the approach and throw vectors to perform a grenade return",
			generate_hash("ai_grenadeReturn_approachMinDot")
		},
		{
			"ai_grenadeReturn_debug",
			"Turns on debug info for AI grenade returns",
			generate_hash("ai_grenadeReturn_debug")
		},
		{
			"ai_grenadeReturn_extraFuseTime",
			"The amount of time (in ms) to add to a grenade fuse when trying to return grenade that's below minFuseTime",
			generate_hash("ai_grenadeReturn_extraFuseTime")
		},
		{
			"ai_grenadeReturn_minDistSqr",
			"Minimal distance to a grenade to consider it for a return so that transition anims will play",
			generate_hash("ai_grenadeReturn_minDistSqr")
		},
		{
			"ai_grenadeReturn_minFuseTime",
			"If the fuse time drops below this value when an ally is attempting to return a grenade, add extra fuse time",
			generate_hash("ai_grenadeReturn_minFuseTime")
		},
		{
			"ai_grenadeReturn_stationary",
			"If set, AI will attempt to return grenades that they are within pickup distance - regardless of min dist",
			generate_hash("ai_grenadeReturn_stationary")
		},
		{
			"ai_grenadeReturn_traceToGrenade",
			"If set, AI will only attempt to return grenades when they have a clear sight trace to the grenade",
			generate_hash("ai_grenadeReturn_traceToGrenade")
		},
		{
			"ai_threatUpdateInterval",
			"AI target threat update interval in milliseconds",
			generate_hash("ai_threatUpdateInterval")
		},
		{
			"aim_autoaim_enabled",
			"",
			generate_hash("aim_autoaim_enabled")
		},
		{
			"aim_target_sentient_radius",
			"The radius used to calculate target bounds for a sentient(actor or player)",
			generate_hash("aim_target_sentient_radius")
		},
		{
			"ammoCounterHide",
			"Hide the Ammo Counter",
			generate_hash("ammoCounterHide")
		},
		{
			"armory_contentpacks_enabled",
			"Allowed armory content packs. 0: none , 1: first armory content pack enabled, 2: first and second armory content pack enabled",
			generate_hash("armory_contentpacks_enabled")
		},
		{
			"badHost_detectMinServerTime",
			"Time in MS before the bad host dection system kicks in after match start",
			generate_hash("badHost_detectMinServerTime")
		},
		{
			"badhost_maxDoISuckFrames",
			"Max lagged frames need to end match",
			generate_hash("badhost_maxDoISuckFrames")
		},
		{
			"band_12players",
			"12 player bandwidth req'd",
			generate_hash("band_12players")
		},
		{
			"band_18players",
			"18 player bandwidth req'd",
			generate_hash("band_18players")
		},
		{
			"band_2players",
			"2 player bandwidth req'd",
			generate_hash("band_2players")
		},
		{
			"band_4players",
			"4 player bandwidth req'd",
			generate_hash("band_4players")
		},
		{
			"band_8players",
			"8 player bandwidth req'd",
			generate_hash("band_8players")
		},
		{
			"bg_allowScuffFootsteps",
			"If true, scuff sounds will be played when the player rotates in place.",
			generate_hash("bg_allowScuffFootsteps")
		},
		{
			"bg_bulletExplDmgFactor",
			"Weapon damage multiplier that will be applied at the center of the slash damage area.",
			generate_hash("bg_bulletExplDmgFactor")
		},
		{
			"bg_bulletExplRadius",
			"The radius of the bullet splash damage, where the damage gradually falls off to 0.",
			generate_hash("bg_bulletExplRadius")
		},
		{
			"bg_compassShowEnemies",
			"Whether enemies are visible on the compass at all times",
			generate_hash("bg_compassShowEnemies")
		},
		{
			"bg_idleSwingSpeed",
			"The rate at which the player's legs swing around when idle (multi-player only)",
			generate_hash("bg_idleSwingSpeed")
		},
		{
			"bg_shieldHitEncodeHeightVM",
			"The decoding range, in height, of a client's viewmodel shield.",
			generate_hash("bg_shieldHitEncodeHeightVM")
		},
		{
			"bg_shieldHitEncodeHeightWorld",
			"The encoding range, in height, of a client's world shield.  A hit in this range is encoded into one of 8 rows.",
			generate_hash("bg_shieldHitEncodeHeightWorld")
		},
		{
			"bg_shieldHitEncodeWidthVM",
			"The decoding range, in width, of a client's viewmodel shield.",
			generate_hash("bg_shieldHitEncodeWidthVM")
		},
		{
			"bg_shieldHitEncodeWidthWorld",
			"The encoding range, in width, of a client's world shield.  A hit in this range is encoded into one of 16 collumns.",
			generate_hash("bg_shieldHitEncodeWidthWorld")
		},
		{
			"bg_shock_fadeOverride",
			"Override the time for the shellshock kick effect to fade in MP",
			generate_hash("bg_shock_fadeOverride")
		},
		{
			"bg_shock_lookControl",
			"Alter player control during shellshock",
			generate_hash("bg_shock_lookControl")
		},
		{
			"bg_shock_lookControl_fadeTime",
			"The time for the shellshock player control to fade in seconds",
			generate_hash("bg_shock_lookControl_fadeTime")
		},
		{
			"bg_shock_lookControl_maxpitchspeed",
			"Maximum pitch movement rate while shellshocked in degrees per second",
			generate_hash("bg_shock_lookControl_maxpitchspeed")
		},
		{
			"bg_shock_lookControl_maxyawspeed",
			"Maximum yaw movement rate while shell shocked in degrees per second",
			generate_hash("bg_shock_lookControl_maxyawspeed")
		},
		{
			"bg_shock_lookControl_mousesensitivityscale",
			"Sensitivity scale to apply to a shellshocked player",
			generate_hash("bg_shock_lookControl_mousesensitivityscale")
		},
		{
			"bg_shock_movement",
			"Affect player's movement speed duringi shellshock",
			generate_hash("bg_shock_movement")
		},
		{
			"bg_shock_screenBlurBlendFadeTime",
			"The amount of time in seconds for the shellshock effect to fade",
			generate_hash("bg_shock_screenBlurBlendFadeTime")
		},
		{
			"bg_shock_screenBlurBlendTime",
			"The amount of time in seconds for the shellshock effect to fade",
			generate_hash("bg_shock_screenBlurBlendTime")
		},
		{
			"bg_shock_screenFlashShotFadeTime",
			"In seconds, how soon from the end of the effect to start blending out the whiteout layer.",
			generate_hash("bg_shock_screenFlashShotFadeTime")
		},
		{
			"bg_shock_screenFlashWhiteFadeTime",
			"In seconds, how soon from the end of the effect to start blending out the whiteout layer.",
			generate_hash("bg_shock_screenFlashWhiteFadeTime")
		},
		{
			"bg_shock_screenType",
			"Shell shock screen effect type",
			generate_hash("bg_shock_screenType")
		},
		{
			"bg_shock_sound",
			"Play shell shock sound",
			generate_hash("bg_shock_sound")
		},
		{
			"bg_shock_soundDryLevel",
			"Shell shock sound dry level",
			generate_hash("bg_shock_soundDryLevel")
		},
		{
			"bg_shock_soundEnd",
			"Shellshock end sound alias",
			generate_hash("bg_shock_soundEnd")
		},
		{
			"bg_shock_soundEndAbort",
			"Shellshock aborted end sound alias",
			generate_hash("bg_shock_soundEndAbort")
		},
		{
			"bg_shock_soundFadeInTime",
			"Shell shock sound fade in time in seconds",
			generate_hash("bg_shock_soundFadeInTime")
		},
		{
			"bg_shock_soundFadeOutTime",
			"Shell shock sound fade out time in seconds",
			generate_hash("bg_shock_soundFadeOutTime")
		},
		{
			"bg_shock_soundLoop",
			"Shellshock loop alias",
			generate_hash("bg_shock_soundLoop")
		},
		{
			"bg_shock_soundLoopEndDelay",
			"Sound loop end offset time from the end of the shellshock in seconds",
			generate_hash("bg_shock_soundLoopEndDelay")
		},
		{
			"bg_shock_soundLoopFadeTime",
			"Shell shock sound loop fade time in seconds",
			generate_hash("bg_shock_soundLoopFadeTime")
		},
		{
			"bg_shock_soundLoopSilent",
			"The sound that gets blended with the shellshock loop alias",
			generate_hash("bg_shock_soundLoopSilent")
		},
		{
			"bg_shock_soundModEndDelay",
			"The delay from the end of the shell shock to the end of the sound modification",
			generate_hash("bg_shock_soundModEndDelay")
		},
		{
			"bg_shock_soundRoomType",
			"Shell shock sound reverb room type",
			generate_hash("bg_shock_soundRoomType")
		},
		{
			"bg_shock_soundSubmix",
			"Shell shock submix to apply",
			generate_hash("bg_shock_soundSubmix")
		},
		{
			"bg_shock_soundWetLevel",
			"Shell shock sound wet level",
			generate_hash("bg_shock_soundWetLevel")
		},
		{
			"bg_shock_viewKickFadeTime",
			"The time for the shellshock kick effect to fade",
			generate_hash("bg_shock_viewKickFadeTime")
		},
		{
			"bg_shock_viewKickPeriod",
			"The period of the shellshock view kick effect",
			generate_hash("bg_shock_viewKickPeriod")
		},
		{
			"bg_shock_viewKickRadius",
			"Shell shock kick radius",
			generate_hash("bg_shock_viewKickRadius")
		},
		{
			"bg_swingSpeed",
			"The rate at which the player's legs swing around when idle (multi-player only)",
			generate_hash("bg_swingSpeed")
		},
		{
			"bg_torsoSwingSpeed",
			"The rate at which the player's torso swings around when strafing (multi-player only)",
			generate_hash("bg_torsoSwingSpeed")
		},
		{
			"boostcheatHeadshotsTotalCoef",
			"",
			generate_hash("boostcheatHeadshotsTotalCoef")
		},
		{
			"boostcheatHeadshotsTotalMean",
			"",
			generate_hash("boostcheatHeadshotsTotalMean")
		},
		{
			"boostcheatHeadshotsTotalStddev",
			"",
			generate_hash("boostcheatHeadshotsTotalStddev")
		},
		{
			"boostcheatIntercept",
			"",
			generate_hash("boostcheatIntercept")
		},
		{
			"boostcheatKillerXAnomalyCoef",
			"",
			generate_hash("boostcheatKillerXAnomalyCoef")
		},
		{
			"boostcheatKillerXAnomalyMean",
			"",
			generate_hash("boostcheatKillerXAnomalyMean")
		},
		{
			"boostcheatKillerXAnomalyStddev",
			"",
			generate_hash("boostcheatKillerXAnomalyStddev")
		},
		{
			"boostcheatKillerYAnomalyCoef",
			"",
			generate_hash("boostcheatKillerYAnomalyCoef")
		},
		{
			"boostcheatKillerYAnomalyMean",
			"",
			generate_hash("boostcheatKillerYAnomalyMean")
		},
		{
			"boostcheatKillerYAnomalyStddev",
			"",
			generate_hash("boostcheatKillerYAnomalyStddev")
		},
		{
			"boostcheatMeanDistanceMostKilledPlayerTraveledMean",
			"",
			generate_hash("boostcheatMeanDistanceMostKilledPlayerTraveledMean")
		},
		{
			"boostcheatMeanDistanceVictimTraveledCoef",
			"",
			generate_hash("boostcheatMeanDistanceVictimTraveledCoef")
		},
		{
			"boostcheatMeanDistanceVictimTraveledMean",
			"",
			generate_hash("boostcheatMeanDistanceVictimTraveledMean")
		},
		{
			"boostcheatMeanDistanceVictimTraveledStddev",
			"",
			generate_hash("boostcheatMeanDistanceVictimTraveledStddev")
		},
		{
			"boostcheatMeanMostKilledPlayerLifetimeMillisecondsMean",
			"",
			generate_hash("boostcheatMeanMostKilledPlayerLifetimeMillisecondsMean")
		},
		{
			"boostcheatMostKilledPlayerHKRatioCoef",
			"",
			generate_hash("boostcheatMostKilledPlayerHKRatioCoef")
		},
		{
			"boostcheatMostKilledPlayerHKRatioMean",
			"",
			generate_hash("boostcheatMostKilledPlayerHKRatioMean")
		},
		{
			"boostcheatMostKilledPlayerHKRatioStddev",
			"",
			generate_hash("boostcheatMostKilledPlayerHKRatioStddev")
		},
		{
			"boostcheatMostKilledPlayerKillsRatioCoef",
			"",
			generate_hash("boostcheatMostKilledPlayerKillsRatioCoef")
		},
		{
			"boostcheatMostKilledPlayerKillsRatioMean",
			"",
			generate_hash("boostcheatMostKilledPlayerKillsRatioMean")
		},
		{
			"boostcheatMostKilledPlayerKillsRatioStddev",
			"",
			generate_hash("boostcheatMostKilledPlayerKillsRatioStddev")
		},
		{
			"boostcheatMostKilledPlayerKillsTotalCoef",
			"",
			generate_hash("boostcheatMostKilledPlayerKillsTotalCoef")
		},
		{
			"boostcheatMostKilledPlayerKillsTotalMean",
			"",
			generate_hash("boostcheatMostKilledPlayerKillsTotalMean")
		},
		{
			"boostcheatMostKilledPlayerKillsTotalStddev",
			"",
			generate_hash("boostcheatMostKilledPlayerKillsTotalStddev")
		},
		{
			"boostcheatMostKilledPlayerKillTimestampsAnomalyMean",
			"",
			generate_hash("boostcheatMostKilledPlayerKillTimestampsAnomalyMean")
		},
		{
			"boostcheatVictimXAnomalyCoef",
			"",
			generate_hash("boostcheatVictimXAnomalyCoef")
		},
		{
			"boostcheatVictimXAnomalyMean",
			"",
			generate_hash("boostcheatVictimXAnomalyMean")
		},
		{
			"boostcheatVictimXAnomalyStddev",
			"",
			generate_hash("boostcheatVictimXAnomalyStddev")
		},
		{
			"boostcheatVictimYAnomalyCoef",
			"",
			generate_hash("boostcheatVictimYAnomalyCoef")
		},
		{
			"boostcheatVictimYAnomalyMean",
			"",
			generate_hash("boostcheatVictimYAnomalyMean")
		},
		{
			"boostcheatVictimYAnomalyStddev",
			"",
			generate_hash("boostcheatVictimYAnomalyStddev")
		},
		{
			"bot_DifficultyDefault",
			"default difficulty level of bots",
			generate_hash("bot_DifficultyDefault")
		},
		{
			"ca_auto_signin",
			"CoD Anywhere start sign-in task automatically on startup or first party sign-in",
			generate_hash("ca_auto_signin")
		},
		{
			"ca_do_mlc",
			"CoD Anywhere Do Multi Login check",
			generate_hash("ca_do_mlc")
		},
		{
			"ca_intra_only",
			"CoD Anywhere Intra Network Only",
			generate_hash("ca_intra_only")
		},
		{
			"ca_require_signin",
			"CoD Anywhere require sign in to enter MP",
			generate_hash("ca_require_signin")
		},
		{
			"ca_show_signup_request",
			"CoD Anywhere should you show new users a popup requesting they create a CoD Account?",
			generate_hash("ca_show_signup_request")
		},
		{
			"camera_thirdPerson",
			"Use third person view globally",
			generate_hash("camera_thirdPerson")
		},
		{
			"cameraShakeRemoteHelo_Angles",
			"Remote helicopter gunner cam, range to shake the view.",
			generate_hash("cameraShakeRemoteHelo_Angles")
		},
		{
			"cameraShakeRemoteHelo_Freqs",
			"Remote helicopter gunner cam, how fast to shake.",
			generate_hash("cameraShakeRemoteHelo_Freqs")
		},
		{
			"cameraShakeRemoteHelo_SpeedRange",
			"Remote helicopter gunner cam, range of missile speed to scale the shaking.",
			generate_hash("cameraShakeRemoteHelo_SpeedRange")
		},
		{
			"cameraShakeRemoteMissile_Angles",
			"Remote missile-cam, range to shake the view.",
			generate_hash("cameraShakeRemoteMissile_Angles")
		},
		{
			"cameraShakeRemoteMissile_Freqs",
			"Remote missile-cam, how fast to shake.",
			generate_hash("cameraShakeRemoteMissile_Freqs")
		},
		{
			"cameraShakeRemoteMissile_SpeedRange",
			"Remote missile-cam, range of missile speed to scale the shaking.",
			generate_hash("cameraShakeRemoteMissile_SpeedRange")
		},
		{
			"cg_airstrikeCamFstop",
			"Airstrike kill camera aperture. Lower f-stop yields a shallower depth of field. Typical values range from 1 to 22",
			generate_hash("cg_airstrikeCamFstop")
		},
		{
			"cg_airstrikeKillCamFarBlur",
			"",
			generate_hash("cg_airstrikeKillCamFarBlur")
		},
		{
			"cg_airstrikeKillCamFarBlurDist",
			"",
			generate_hash("cg_airstrikeKillCamFarBlurDist")
		},
		{
			"cg_airstrikeKillCamFarBlurStart",
			"",
			generate_hash("cg_airstrikeKillCamFarBlurStart")
		},
		{
			"cg_airstrikeKillCamFov",
			"Airstrike kill camera field of view.",
			generate_hash("cg_airstrikeKillCamFov")
		},
		{
			"cg_airstrikeKillCamNearBlur",
			"",
			generate_hash("cg_airstrikeKillCamNearBlur")
		},
		{
			"cg_airstrikeKillCamNearBlurEnd",
			"",
			generate_hash("cg_airstrikeKillCamNearBlurEnd")
		},
		{
			"cg_airstrikeKillCamNearBlurStart",
			"",
			generate_hash("cg_airstrikeKillCamNearBlurStart")
		},
		{
			"cg_blood",
			"Show Blood",
			generate_hash("cg_blood")
		},
		{
			"cg_bloodThickColor",
			"Color of the blood overlay's thick blood splatter",
			generate_hash("cg_bloodThickColor")
		},
		{
			"cg_bloodThinColor",
			"Color of the blood overlay's thin blood splatter",
			generate_hash("cg_bloodThinColor")
		},
		{
			"cg_brass",
			"Weapons eject brass",
			generate_hash("cg_brass")
		},
		{
			"cg_centertime",
			"The time for a center printed message to fade",
			generate_hash("cg_centertime")
		},
		{
			"cg_chatHeight",
			"The font height of a chat message",
			generate_hash("cg_chatHeight")
		},
		{
			"cg_chatTime",
			"The amount of time that a chat message is visible",
			generate_hash("cg_chatTime")
		},
		{
			"cg_ColorBlind_EnemyTeam",
			"Enemy team color for color blind people",
			generate_hash("cg_ColorBlind_EnemyTeam")
		},
		{
			"cg_ColorBlind_MyParty",
			"Player party color for color blind people",
			generate_hash("cg_ColorBlind_MyParty")
		},
		{
			"cg_ColorBlind_MyTeam",
			"Player team color for color blind people",
			generate_hash("cg_ColorBlind_MyTeam")
		},
		{
			"cg_connectionIconSize",
			"Size of the connection icon",
			generate_hash("cg_connectionIconSize")
		},
		{
			"cg_constantSizeHeadIcons",
			"Head icons are the same size regardless of distance from the player",
			generate_hash("cg_constantSizeHeadIcons")
		},
		{
			"cg_crosshairAlpha",
			"The alpha value of the crosshair",
			generate_hash("cg_crosshairAlpha")
		},
		{
			"cg_crosshairAlphaMin",
			"The minimum alpha value of the crosshair when it fades in",
			generate_hash("cg_crosshairAlphaMin")
		},
		{
			"cg_crosshairDynamic",
			"Crosshair is Dynamic",
			generate_hash("cg_crosshairDynamic")
		},
		{
			"cg_crosshairEnemyColor",
			"The crosshair color when over an enemy",
			generate_hash("cg_crosshairEnemyColor")
		},
		{
			"cg_crosshairVerticalOffset",
			"Amount to vertically offset the crosshair from the center.",
			generate_hash("cg_crosshairVerticalOffset")
		},
		{
			"cg_cullBulletAngle",
			"Cull bullet trajectories that don't fall within this fov",
			generate_hash("cg_cullBulletAngle")
		},
		{
			"cg_cullBullets",
			"Whether to cull bullet fire prediction if trajectory doesn't pass your view or anywhere near you",
			generate_hash("cg_cullBullets")
		},
		{
			"cg_cursorHints",
			"Draw cursor hints where:\n 0: no hints",
			generate_hash("cg_cursorHints")
		},
		{
			"cg_deadChatWithDead",
			"If true, dead players can all chat together, regardless of team",
			generate_hash("cg_deadChatWithDead")
		},
		{
			"cg_deadChatWithTeam",
			"If true, dead players can talk to living players on their team",
			generate_hash("cg_deadChatWithTeam")
		},
		{
			"cg_deadHearAllLiving",
			"If true, dead players can hear all living players talk",
			generate_hash("cg_deadHearAllLiving")
		},
		{
			"cg_deadHearTeamLiving",
			"If true, dead players can hear living players on their team talk",
			generate_hash("cg_deadHearTeamLiving")
		},
		{
			"cg_descriptiveText",
			"Draw descriptive spectator messages",
			generate_hash("cg_descriptiveText")
		},
		{
			"cg_draw2D",
			"Draw 2D screen elements",
			generate_hash("cg_draw2D")
		},
		{
			"cg_drawBreathHint",
			"Draw a 'hold breath to steady' hint",
			generate_hash("cg_drawBreathHint")
		},
		{
			"cg_drawBuildName",
			"Draw build name",
			generate_hash("cg_drawBuildName")
		},
		{
			"cg_drawCrosshair",
			"Turn on weapon crosshair",
			generate_hash("cg_drawCrosshair")
		},
		{
			"cg_drawCrosshairNames",
			"Draw the name of an enemy under the crosshair",
			generate_hash("cg_drawCrosshairNames")
		},
		{
			"cg_drawCrosshairNamesPosX",
			"",
			generate_hash("cg_drawCrosshairNamesPosX")
		},
		{
			"cg_drawCrosshairNamesPosY",
			"",
			generate_hash("cg_drawCrosshairNamesPosY")
		},
		{
			"cg_drawDamageDirection",
			"Draw hit direction arrow.",
			generate_hash("cg_drawDamageDirection")
		},
		{
			"cg_drawDamageFlash",
			"Draw flash when hit.",
			generate_hash("cg_drawDamageFlash")
		},
		{
			"cg_drawDoubleTapDetonateHint",
			"Draw a 'double tap to detonate grenade' hint",
			generate_hash("cg_drawDoubleTapDetonateHint")
		},
		{
			"cg_drawEffectNum",
			"Draw counts of effects and elements",
			generate_hash("cg_drawEffectNum")
		},
		{
			"cg_drawFPS",
			"Draw frames per second",
			generate_hash("cg_drawFPS")
		},
		{
			"cg_drawFPSLabels",
			"Draw FPS Info Labels",
			generate_hash("cg_drawFPSLabels")
		},
		{
			"cg_drawFriendlyHUDGrenades",
			"Draw grenade warning indicators for friendly grenades (should be true if friendly-fire is enabled)",
			generate_hash("cg_drawFriendlyHUDGrenades")
		},
		{
			"cg_drawFriendlyNames",
			"Whether to show friendly names in game",
			generate_hash("cg_drawFriendlyNames")
		},
		{
			"cg_drawFriendlyNamesAlways",
			"Whether to always show friendly names in game (for certain gametypes)",
			generate_hash("cg_drawFriendlyNamesAlways")
		},
		{
			"cg_drawGun",
			"Draw the view model",
			generate_hash("cg_drawGun")
		},
		{
			"cg_drawHealth",
			"Draw health bar",
			generate_hash("cg_drawHealth")
		},
		{
			"cg_drawMantleHint",
			"Draw a 'press key to mantle' hint",
			generate_hash("cg_drawMantleHint")
		},
		{
			"cg_drawMaterial",
			"Draw debugging information for materials",
			generate_hash("cg_drawMaterial")
		},
		{
			"cg_drawpaused",
			"Draw paused screen",
			generate_hash("cg_drawpaused")
		},
		{
			"cg_drawScriptUsage",
			"Draw debugging information for scripts",
			generate_hash("cg_drawScriptUsage")
		},
		{
			"cg_drawSnapshot",
			"Draw debugging information for snapshots",
			generate_hash("cg_drawSnapshot")
		},
		{
			"cg_drawStatsSource",
			"Draw stats source",
			generate_hash("cg_drawStatsSource")
		},
		{
			"cg_drawTalk",
			"Controls which icons CG_TALKER ownerdraw draws",
			generate_hash("cg_drawTalk")
		},
		{
			"cg_drawTurretCrosshair",
			"Draw a cross hair when using a turret",
			generate_hash("cg_drawTurretCrosshair")
		},
		{
			"cg_drawViewpos",
			"Draw viewpos",
			generate_hash("cg_drawViewpos")
		},
		{
			"cg_e3TrailerHacks",
			"Tweaks for trailer recording",
			generate_hash("cg_e3TrailerHacks")
		},
		{
			"cg_equipmentSounds",
			"Play equipment sounds",
			generate_hash("cg_equipmentSounds")
		},
		{
			"cg_errordecay",
			"Decay for predicted error",
			generate_hash("cg_errordecay")
		},
		{
			"cg_everyoneHearsEveryone",
			"If true, all players can all chat together, regardless of team or death",
			generate_hash("cg_everyoneHearsEveryone")
		},
		{
			"cg_explosiveKillCamBackDist",
			"Explosive kill camera: distance of camera backwards from explosive.",
			generate_hash("cg_explosiveKillCamBackDist")
		},
		{
			"cg_explosiveKillCamGroundBackDist",
			"Explosive kill camera when stuck to ground: distance of camera backwards from explosive.",
			generate_hash("cg_explosiveKillCamGroundBackDist")
		},
		{
			"cg_explosiveKillCamGroundUpDist",
			"Explosive kill camera when stuck to ground: distance of camera backwards from explosive.",
			generate_hash("cg_explosiveKillCamGroundUpDist")
		},
		{
			"cg_explosiveKillCamStopDecelDist",
			"Rocket and Grenade Launcher kill camera: distance from player to begin coming to rest",
			generate_hash("cg_explosiveKillCamStopDecelDist")
		},
		{
			"cg_explosiveKillCamStopDist",
			"Rocket and Grenade Launcher kill camera: distance from player to begin coming to rest",
			generate_hash("cg_explosiveKillCamStopDist")
		},
		{
			"cg_explosiveKillCamUpDist",
			"Explosive kill camera: distance of camera backwards from explosive.",
			generate_hash("cg_explosiveKillCamUpDist")
		},
		{
			"cg_explosiveKillCamWallOutDist",
			"Explosive kill camera when stuck to wall: distance of camera out from wall.",
			generate_hash("cg_explosiveKillCamWallOutDist")
		},
		{
			"cg_explosiveKillCamWallSideDist",
			"Explosive kill camera when stuck to wall: distance of camera out from wall.",
			generate_hash("cg_explosiveKillCamWallSideDist")
		},
		{
			"cg_flashbangNameFadeIn",
			"Time in milliseconds to fade in friendly names",
			generate_hash("cg_flashbangNameFadeIn")
		},
		{
			"cg_flashbangNameFadeOut",
			"Time in milliseconds to fade out friendly names",
			generate_hash("cg_flashbangNameFadeOut")
		},
		{
			"cg_foliagesnd_alias",
			"The sound that plays when an actor or player enters a foliage clip brush.",
			generate_hash("cg_foliagesnd_alias")
		},
		{
			"cg_footsteps",
			"Play footstep sounds that are NOT sprint",
			generate_hash("cg_footsteps")
		},
		{
			"cg_footstepsSprint",
			"Play sprint footstep sounds",
			generate_hash("cg_footstepsSprint")
		},
		{
			"cg_fov",
			"The field of view angle in degrees",
			generate_hash("cg_fov")
		},
		{
			"cg_fovMin",
			"The minimum possible field of view",
			generate_hash("cg_fovMin")
		},
		{
			"cg_fovScale",
			"Scale applied to the field of view",
			generate_hash("cg_fovScale")
		},
		{
			"cg_friendlyNameFadeIn",
			"Time in milliseconds to fade in friendly names",
			generate_hash("cg_friendlyNameFadeIn")
		},
		{
			"cg_friendlyNameFadeOut",
			"Time in milliseconds to fade out friendly names",
			generate_hash("cg_friendlyNameFadeOut")
		},
		{
			"cg_gameBoldMessageWidth",
			"The maximum character width of the bold game messages",
			generate_hash("cg_gameBoldMessageWidth")
		},
		{
			"cg_gameMessageWidth",
			"The maximum character width of the game messages",
			generate_hash("cg_gameMessageWidth")
		},
		{
			"cg_gun_x",
			"Forward position of the viewmodel",
			generate_hash("cg_gun_x")
		},
		{
			"cg_gun_y",
			"Right position of the viewmodel",
			generate_hash("cg_gun_y")
		},
		{
			"cg_gun_z",
			"Up position of the viewmodel",
			generate_hash("cg_gun_z")
		},
		{
			"cg_headIconMinScreenRadius",
			"The minumum radius of a head icon on the screen",
			generate_hash("cg_headIconMinScreenRadius")
		},
		{
			"cg_hearKillerTime",
			"Duration (in milliseconds) to hear the person you just killed",
			generate_hash("cg_hearKillerTime")
		},
		{
			"cg_hearVictimEnabled",
			"If true, you can hear the person you just killed",
			generate_hash("cg_hearVictimEnabled")
		},
		{
			"cg_hearVictimTime",
			"Duration (in milliseconds) to hear the person you just killed",
			generate_hash("cg_hearVictimTime")
		},
		{
			"cg_heliKillCamFarBlur",
			"",
			generate_hash("cg_heliKillCamFarBlur")
		},
		{
			"cg_heliKillCamFarBlurDist",
			"",
			generate_hash("cg_heliKillCamFarBlurDist")
		},
		{
			"cg_heliKillCamFarBlurStart",
			"",
			generate_hash("cg_heliKillCamFarBlurStart")
		},
		{
			"cg_heliKillCamFov",
			"Helicopter kill camera field of view.",
			generate_hash("cg_heliKillCamFov")
		},
		{
			"cg_heliKillCamFstop",
			"Helicopter kill camera aperture. Lower f-stop yields a shallower depth of field. Typical values range from 1 to 22",
			generate_hash("cg_heliKillCamFstop")
		},
		{
			"cg_heliKillCamNearBlur",
			"",
			generate_hash("cg_heliKillCamNearBlur")
		},
		{
			"cg_heliKillCamNearBlurEnd",
			"",
			generate_hash("cg_heliKillCamNearBlurEnd")
		},
		{
			"cg_heliKillCamNearBlurStart",
			"",
			generate_hash("cg_heliKillCamNearBlurStart")
		},
		{
			"cg_hintFadeTime",
			"Time in milliseconds for the cursor hint to fade",
			generate_hash("cg_hintFadeTime")
		},
		{
			"cg_hudChatIntermissionPosition",
			"Position of the HUD chat box during intermission",
			generate_hash("cg_hudChatIntermissionPosition")
		},
		{
			"cg_hudChatPosition",
			"Position of the HUD chat box",
			generate_hash("cg_hudChatPosition")
		},
		{
			"cg_hudDamageIconHeight",
			"The height of the damage icon",
			generate_hash("cg_hudDamageIconHeight")
		},
		{
			"cg_hudDamageIconInScope",
			"Draw damage icons when aiming down the sight of a scoped weapon",
			generate_hash("cg_hudDamageIconInScope")
		},
		{
			"cg_hudDamageIconOffset",
			"The offset from the center of the damage icon",
			generate_hash("cg_hudDamageIconOffset")
		},
		{
			"cg_hudDamageIconOverlayTime",
			"The amount of time (in ms) for the overlay portion of the damage icon to stay on screen",
			generate_hash("cg_hudDamageIconOverlayTime")
		},
		{
			"cg_hudDamageIconStartFadeTime",
			"The amount of time (in ms) before the damage icon begins to fade",
			generate_hash("cg_hudDamageIconStartFadeTime")
		},
		{
			"cg_hudDamageIconTime",
			"The amount of time for the damage icon to stay on screen after damage is taken",
			generate_hash("cg_hudDamageIconTime")
		},
		{
			"cg_hudDamageIconWidth",
			"The width of the damage icon",
			generate_hash("cg_hudDamageIconWidth")
		},
		{
			"cg_hudGrenadeIconEnabledFlash",
			"Show the grenade indicator for flash grenades",
			generate_hash("cg_hudGrenadeIconEnabledFlash")
		},
		{
			"cg_hudGrenadeIconHeight",
			"The height of the grenade indicator icon",
			generate_hash("cg_hudGrenadeIconHeight")
		},
		{
			"cg_hudGrenadeIconInScope",
			"Show the grenade indicator when aiming down the sight of a scoped weapon",
			generate_hash("cg_hudGrenadeIconInScope")
		},
		{
			"cg_hudGrenadeIconMaxRangeFlash",
			"The minimum distance that a flashbang has to be from a player in order to be shown on the grenade indicator",
			generate_hash("cg_hudGrenadeIconMaxRangeFlash")
		},
		{
			"cg_hudGrenadeIconMaxRangeFrag",
			"The minimum distance that a grenade has to be from a player in order to be shown on the grenade indicator",
			generate_hash("cg_hudGrenadeIconMaxRangeFrag")
		},
		{
			"cg_hudGrenadeIconOffset",
			"The offset from the center of the screen for a grenade icon",
			generate_hash("cg_hudGrenadeIconOffset")
		},
		{
			"cg_hudGrenadeIconWidth",
			"The width of the grenade indicator icon",
			generate_hash("cg_hudGrenadeIconWidth")
		},
		{
			"cg_hudGrenadePointerHeight",
			"The height of the grenade indicator pointer",
			generate_hash("cg_hudGrenadePointerHeight")
		},
		{
			"cg_hudGrenadePointerPivot",
			"The pivot point of th grenade indicator pointer",
			generate_hash("cg_hudGrenadePointerPivot")
		},
		{
			"cg_hudGrenadePointerPulseFreq",
			"The number of times per second that the grenade indicator flashes in Hertz",
			generate_hash("cg_hudGrenadePointerPulseFreq")
		},
		{
			"cg_hudGrenadePointerPulseMax",
			"The maximum alpha of the grenade indicator pulse. Values higher than 1 will cause the indicator to remain at full brightness for longer",
			generate_hash("cg_hudGrenadePointerPulseMax")
		},
		{
			"cg_hudGrenadePointerPulseMin",
			"The minimum alpha of the grenade indicator pulse. Values lower than 0 will cause the indicator to remain at full transparency for longer",
			generate_hash("cg_hudGrenadePointerPulseMin")
		},
		{
			"cg_hudGrenadePointerWidth",
			"The width of the grenade indicator pointer",
			generate_hash("cg_hudGrenadePointerWidth")
		},
		{
			"cg_hudLegacySplitscreenScale",
			"Screen scale for hud elements in splitscreen",
			generate_hash("cg_hudLegacySplitscreenScale")
		},
		{
			"cg_hudLighting_basic_additiveLumOffset",
			"[basic] Offset applied to additive light color.",
			generate_hash("cg_hudLighting_basic_additiveLumOffset")
		},
		{
			"cg_hudLighting_basic_additiveLumScale",
			"[basic] Scale applied to additive light color.",
			generate_hash("cg_hudLighting_basic_additiveLumScale")
		},
		{
			"cg_hudLighting_basic_additiveOffset",
			"",
			generate_hash("cg_hudLighting_basic_additiveOffset")
		},
		{
			"cg_hudLighting_basic_additiveScale",
			"",
			generate_hash("cg_hudLighting_basic_additiveScale")
		},
		{
			"cg_hudLighting_basic_ambientLumOffset",
			"[basic] Offset applied to ambient light color.",
			generate_hash("cg_hudLighting_basic_ambientLumOffset")
		},
		{
			"cg_hudLighting_basic_ambientLumScale",
			"[basic] Scale applied to ambient light color.",
			generate_hash("cg_hudLighting_basic_ambientLumScale")
		},
		{
			"cg_hudLighting_basic_ambientOffset",
			"",
			generate_hash("cg_hudLighting_basic_ambientOffset")
		},
		{
			"cg_hudLighting_basic_ambientScale",
			"",
			generate_hash("cg_hudLighting_basic_ambientScale")
		},
		{
			"cg_hudLighting_basic_diffuseLumOffset",
			"[basic] Offset applied to diffuse light color.",
			generate_hash("cg_hudLighting_basic_diffuseLumOffset")
		},
		{
			"cg_hudLighting_basic_diffuseLumScale",
			"[basic] Scale applied to diffuse light color.",
			generate_hash("cg_hudLighting_basic_diffuseLumScale")
		},
		{
			"cg_hudLighting_basic_diffuseOffset",
			"",
			generate_hash("cg_hudLighting_basic_diffuseOffset")
		},
		{
			"cg_hudLighting_basic_diffuseScale",
			"",
			generate_hash("cg_hudLighting_basic_diffuseScale")
		},
		{
			"cg_hudLighting_basic_specExponent",
			"[basic] Specular exponent.  Higher values result in sharper highlights.",
			generate_hash("cg_hudLighting_basic_specExponent")
		},
		{
			"cg_hudLighting_basic_specLumOffset",
			"[basic] Offset applied to spec light luminance.",
			generate_hash("cg_hudLighting_basic_specLumOffset")
		},
		{
			"cg_hudLighting_basic_specLumScale",
			"[basic] Scale applied to spec light luminance.",
			generate_hash("cg_hudLighting_basic_specLumScale")
		},
		{
			"cg_hudLighting_basic_specOffset",
			"",
			generate_hash("cg_hudLighting_basic_specOffset")
		},
		{
			"cg_hudLighting_basic_specScale",
			"",
			generate_hash("cg_hudLighting_basic_specScale")
		},
		{
			"cg_hudLighting_blood_additiveLumOffset",
			"[blood] Offset applied to additive light color.",
			generate_hash("cg_hudLighting_blood_additiveLumOffset")
		},
		{
			"cg_hudLighting_blood_additiveLumScale",
			"[blood] Scale applied to additive light color.",
			generate_hash("cg_hudLighting_blood_additiveLumScale")
		},
		{
			"cg_hudLighting_blood_additiveOffset",
			"",
			generate_hash("cg_hudLighting_blood_additiveOffset")
		},
		{
			"cg_hudLighting_blood_additiveScale",
			"",
			generate_hash("cg_hudLighting_blood_additiveScale")
		},
		{
			"cg_hudLighting_blood_ambientLumOffset",
			"[blood] Offset applied to ambient light color.",
			generate_hash("cg_hudLighting_blood_ambientLumOffset")
		},
		{
			"cg_hudLighting_blood_ambientLumScale",
			"[blood] Scale applied to ambient light color.",
			generate_hash("cg_hudLighting_blood_ambientLumScale")
		},
		{
			"cg_hudLighting_blood_ambientOffset",
			"",
			generate_hash("cg_hudLighting_blood_ambientOffset")
		},
		{
			"cg_hudLighting_blood_ambientScale",
			"",
			generate_hash("cg_hudLighting_blood_ambientScale")
		},
		{
			"cg_hudLighting_blood_diffuseLumOffset",
			"[blood] Offset applied to diffuse light color.",
			generate_hash("cg_hudLighting_blood_diffuseLumOffset")
		},
		{
			"cg_hudLighting_blood_diffuseLumScale",
			"[blood] Scale applied to diffuse light color.",
			generate_hash("cg_hudLighting_blood_diffuseLumScale")
		},
		{
			"cg_hudLighting_blood_diffuseOffset",
			"",
			generate_hash("cg_hudLighting_blood_diffuseOffset")
		},
		{
			"cg_hudLighting_blood_diffuseScale",
			"",
			generate_hash("cg_hudLighting_blood_diffuseScale")
		},
		{
			"cg_hudLighting_blood_specExponent",
			"[blood] Specular exponent.  Higher values result in sharper highlights.",
			generate_hash("cg_hudLighting_blood_specExponent")
		},
		{
			"cg_hudLighting_blood_specLumOffset",
			"[blood] Offset applied to spec light luminance.",
			generate_hash("cg_hudLighting_blood_specLumOffset")
		},
		{
			"cg_hudLighting_blood_specLumScale",
			"[blood] Scale applied to spec light luminance.",
			generate_hash("cg_hudLighting_blood_specLumScale")
		},
		{
			"cg_hudLighting_blood_specOffset",
			"",
			generate_hash("cg_hudLighting_blood_specOffset")
		},
		{
			"cg_hudLighting_blood_specScale",
			"",
			generate_hash("cg_hudLighting_blood_specScale")
		},
		{
			"cg_hudLighting_fadeSharpness",
			"This controls how sharp the lines are when fading using the mask alpha.  Higher values are sharper.",
			generate_hash("cg_hudLighting_fadeSharpness")
		},
		{
			"cg_hudMapBorderWidth",
			"The size of the full map's border, filled by the CG_PLAYER_FULLMAP_BORDER ownerdraw",
			generate_hash("cg_hudMapBorderWidth")
		},
		{
			"cg_hudMapFriendlyHeight",
			"",
			generate_hash("cg_hudMapFriendlyHeight")
		},
		{
			"cg_hudMapFriendlyWidth",
			"",
			generate_hash("cg_hudMapFriendlyWidth")
		},
		{
			"cg_hudMapPlayerHeight",
			"",
			generate_hash("cg_hudMapPlayerHeight")
		},
		{
			"cg_hudMapPlayerWidth",
			"",
			generate_hash("cg_hudMapPlayerWidth")
		},
		{
			"cg_hudMapRadarLineThickness",
			"Thickness, relative to the map width, of the radar texture that sweeps across the full screen map",
			generate_hash("cg_hudMapRadarLineThickness")
		},
		{
			"cg_hudObjectiveTextScale",
			"",
			generate_hash("cg_hudObjectiveTextScale")
		},
		{
			"cg_hudProneY",
			"Virtual screen y coordinate of the prone blocked message",
			generate_hash("cg_hudProneY")
		},
		{
			"cg_hudSayPosition",
			"Position of the HUD say box",
			generate_hash("cg_hudSayPosition")
		},
		{
			"cg_hudSplitscreenCompassElementScale",
			"Scale value to apply to compass elements in splitscreen",
			generate_hash("cg_hudSplitscreenCompassElementScale")
		},
		{
			"cg_hudSplitscreenCompassScale",
			"Scale value to apply to the compass in splitscreen",
			generate_hash("cg_hudSplitscreenCompassScale")
		},
		{
			"cg_hudSplitscreenStanceScale",
			"Scale value to apply to the stance HUD element in splitscreen",
			generate_hash("cg_hudSplitscreenStanceScale")
		},
		{
			"cg_hudStanceFlash",
			"The background color of the flash when the stance changes",
			generate_hash("cg_hudStanceFlash")
		},
		{
			"cg_hudVotePosition",
			"Position of the HUD vote box",
			generate_hash("cg_hudVotePosition")
		},
		{
			"cg_invalidCmdHintBlinkInterval",
			"Blink rate of an invalid command hint",
			generate_hash("cg_invalidCmdHintBlinkInterval")
		},
		{
			"cg_invalidCmdHintDuration",
			"Duration of an invalid command hint",
			generate_hash("cg_invalidCmdHintDuration")
		},
		{
			"cg_javelinKillCamCloseZDist",
			"Javelin kill camera: closest distance above the target.",
			generate_hash("cg_javelinKillCamCloseZDist")
		},
		{
			"cg_javelinKillCamDownDist",
			"Javelin kill camera: distance to follow during ascent.",
			generate_hash("cg_javelinKillCamDownDist")
		},
		{
			"cg_javelinKillCamFov",
			"Javelin kill camera: fov",
			generate_hash("cg_javelinKillCamFov")
		},
		{
			"cg_javelinKillCamLookLerpDist",
			"Javelin kill camera: distance over which to lerp to look at player during descent.  A value of zero means don't lerp at all.",
			generate_hash("cg_javelinKillCamLookLerpDist")
		},
		{
			"cg_javelinKillCamPassDist",
			"Javelin kill camera: distance away when passing.",
			generate_hash("cg_javelinKillCamPassDist")
		},
		{
			"cg_javelinKillCamPassTime",
			"Javelin kill camera: time in seconds to pass javelin on the way up",
			generate_hash("cg_javelinKillCamPassTime")
		},
		{
			"cg_javelinKillCamUpDist",
			"Javelin kill camera: distance to follow during ascent.",
			generate_hash("cg_javelinKillCamUpDist")
		},
		{
			"cg_killCamDefaultLerpTime",
			"Default time used to lerp between killcam entities.",
			generate_hash("cg_killCamDefaultLerpTime")
		},
		{
			"cg_killCamTurretLerpTime",
			"Time used to lerp to a killcam entity of the TURRET type.",
			generate_hash("cg_killCamTurretLerpTime")
		},
		{
			"cg_landingSounds",
			"Play landing on surface sounds",
			generate_hash("cg_landingSounds")
		},
		{
			"cg_largeExplosiveKillCamBackDist",
			"Large Explosive kill camera: distance of camera backwards from explosive.",
			generate_hash("cg_largeExplosiveKillCamBackDist")
		},
		{
			"cg_largeExplosiveKillCamUpDist",
			"Large Explosive kill camera: distance of camera backwards from explosive.",
			generate_hash("cg_largeExplosiveKillCamUpDist")
		},
		{
			"cg_mapLocationSelectionCursorSpeed",
			"Speed of the cursor when selecting a location on the map",
			generate_hash("cg_mapLocationSelectionCursorSpeed")
		},
		{
			"cg_marks_ents_player_only",
			"Marks on entities from players' bullets only.",
			generate_hash("cg_marks_ents_player_only")
		},
		{
			"cg_minCullBulletDist",
			"Don't cull bullet trajectories that are within this distance to you.",
			generate_hash("cg_minCullBulletDist")
		},
		{
			"cg_objectiveText",
			"",
			generate_hash("cg_objectiveText")
		},
		{
			"cg_overheadIconSize",
			"The maximum size to show overhead icons like 'rank'",
			generate_hash("cg_overheadIconSize")
		},
		{
			"cg_overheadNamesFarDist",
			"The far distance at which name sizes are scaled by cg_overheadNamesFarScale",
			generate_hash("cg_overheadNamesFarDist")
		},
		{
			"cg_overheadNamesFarScale",
			"The amount to scale overhead name sizes at cg_overheadNamesFarDist",
			generate_hash("cg_overheadNamesFarScale")
		},
		{
			"cg_overheadNamesFont",
			"Font for overhead names ( see menudefinition.h )",
			generate_hash("cg_overheadNamesFont")
		},
		{
			"cg_overheadNamesGlow",
			"Glow color for overhead names",
			generate_hash("cg_overheadNamesGlow")
		},
		{
			"cg_overheadNamesMaxDist",
			"The maximum distance for showing friendly player names",
			generate_hash("cg_overheadNamesMaxDist")
		},
		{
			"cg_overheadNamesNearDist",
			"The near distance at which names are full size",
			generate_hash("cg_overheadNamesNearDist")
		},
		{
			"cg_overheadNamesSize",
			"The maximum size to show overhead names",
			generate_hash("cg_overheadNamesSize")
		},
		{
			"cg_overheadRankSize",
			"The size to show rank text",
			generate_hash("cg_overheadRankSize")
		},
		{
			"cg_remoteMissileKillCamBackDist",
			"Remote missile kill camera: distance of camera backwards from rocket.",
			generate_hash("cg_remoteMissileKillCamBackDist")
		},
		{
			"cg_remoteMissileKillCamUpDist",
			"Remote missile kill camera: distance of camera backwards from rocket.",
			generate_hash("cg_remoteMissileKillCamUpDist")
		},
		{
			"cg_rocketKillCamBackDist",
			"Rocket kill camera: distance of camera backwards from rocket.",
			generate_hash("cg_rocketKillCamBackDist")
		},
		{
			"cg_rocketKillCamUpDist",
			"Rocket kill camera: distance of camera backwards from rocket.",
			generate_hash("cg_rocketKillCamUpDist")
		},
		{
			"cg_scriptIconSize",
			"Size of Icons defined by script",
			generate_hash("cg_scriptIconSize")
		},
		{
			"cg_showmiss",
			"Show prediction errors",
			generate_hash("cg_showmiss")
		},
		{
			"cg_sprintMeterDisabledColor",
			"The color of the sprint meter when the sprint meter is full",
			generate_hash("cg_sprintMeterDisabledColor")
		},
		{
			"cg_sprintMeterEmptyColor",
			"The color of the sprint meter when the sprint meter is full",
			generate_hash("cg_sprintMeterEmptyColor")
		},
		{
			"cg_sprintMeterFullColor",
			"The color of the sprint meter when the sprint meter is full",
			generate_hash("cg_sprintMeterFullColor")
		},
		{
			"cg_subtitleMinTime",
			"The minimum time that the subtitles are displayed on screen in seconds",
			generate_hash("cg_subtitleMinTime")
		},
		{
			"cg_subtitleWidthStandard",
			"The width of the subtitles on a non wide-screen",
			generate_hash("cg_subtitleWidthStandard")
		},
		{
			"cg_subtitleWidthWidescreen",
			"The width of the subtitle on a wide-screen",
			generate_hash("cg_subtitleWidthWidescreen")
		},
		{
			"cg_teamChatsOnly",
			"Allow chatting only on the same team",
			generate_hash("cg_teamChatsOnly")
		},
		{
			"cg_TeamColor_Allies",
			"Allies team color",
			generate_hash("cg_TeamColor_Allies")
		},
		{
			"cg_TeamColor_Axis",
			"Axis team color",
			generate_hash("cg_TeamColor_Axis")
		},
		{
			"cg_TeamColor_EnemyTeam",
			"Enemy team color",
			generate_hash("cg_TeamColor_EnemyTeam")
		},
		{
			"cg_TeamColor_Free",
			"Free Team color",
			generate_hash("cg_TeamColor_Free")
		},
		{
			"cg_TeamColor_MyParty",
			"Player team color when in the same party",
			generate_hash("cg_TeamColor_MyParty")
		},
		{
			"cg_TeamColor_MyTeam",
			"Player team color",
			generate_hash("cg_TeamColor_MyTeam")
		},
		{
			"cg_TeamColor_Spectator",
			"Spectator team color",
			generate_hash("cg_TeamColor_Spectator")
		},
		{
			"cg_turretKillCamBackDist",
			"Turret kill camera: distance of camera backwards from Turret.",
			generate_hash("cg_turretKillCamBackDist")
		},
		{
			"cg_turretKillCamFov",
			"Turret kill camera field of view.",
			generate_hash("cg_turretKillCamFov")
		},
		{
			"cg_turretKillCamUpDist",
			"Turret kill camera: distance of camera backwards from Turret.",
			generate_hash("cg_turretKillCamUpDist")
		},
		{
			"cg_turretRemoteKillCamBackDist",
			"Remote Turret kill camera: distance of camera backwards from Turret.",
			generate_hash("cg_turretRemoteKillCamBackDist")
		},
		{
			"cg_turretRemoteKillCamFov",
			"Remote Turret kill camera field of view.",
			generate_hash("cg_turretRemoteKillCamFov")
		},
		{
			"cg_turretRemoteKillCamUpDist",
			"Remote Turret kill camera: distance of camera backwards from Turret.",
			generate_hash("cg_turretRemoteKillCamUpDist")
		},
		{
			"cg_vectorFieldsForceUniform",
			"Forces all vector field assets to represent a single, uniform direction",
			generate_hash("cg_vectorFieldsForceUniform")
		},
		{
			"cg_viewVehicleInfluence",
			"The influence on the view angles from being in a vehicle",
			generate_hash("cg_viewVehicleInfluence")
		},
		{
			"cg_viewZSmoothingMax",
			"Threshhold for the maximum smoothing distance we'll do",
			generate_hash("cg_viewZSmoothingMax")
		},
		{
			"cg_viewZSmoothingMin",
			"Threshhold for the minimum smoothing distance it must move to smooth",
			generate_hash("cg_viewZSmoothingMin")
		},
		{
			"cg_viewZSmoothingTime",
			"Amount of time to spread the smoothing over",
			generate_hash("cg_viewZSmoothingTime")
		},
		{
			"cg_voiceIconSize",
			"Size of the 'voice' icon",
			generate_hash("cg_voiceIconSize")
		},
		{
			"cg_waterSheeting_distortionScaleFactor",
			"Distortion uv scales (Default to 1)",
			generate_hash("cg_waterSheeting_distortionScaleFactor")
		},
		{
			"cg_waterSheeting_magnitude",
			"Distortion magnitude",
			generate_hash("cg_waterSheeting_magnitude")
		},
		{
			"cg_waterSheeting_radius",
			"Tweak dev var; Glow radius in pixels at 640x480",
			generate_hash("cg_waterSheeting_radius")
		},
		{
			"cg_weapHitCullAngle",
			"Angle of cone within which to cull back facing weapon hit effects",
			generate_hash("cg_weapHitCullAngle")
		},
		{
			"cg_weapHitCullEnable",
			"When true, cull back facing weapon hit fx.",
			generate_hash("cg_weapHitCullEnable")
		},
		{
			"cg_weaponCycleDelay",
			"The delay after cycling to a new weapon to prevent holding down the cycle weapon button from cycling too fast",
			generate_hash("cg_weaponCycleDelay")
		},
		{
			"cg_weaponHintsCoD1Style",
			"Draw weapon hints in CoD1 style: with the weapon name, and with the icon below",
			generate_hash("cg_weaponHintsCoD1Style")
		},
		{
			"cg_weaponVisInterval",
			"Do weapon vis checks once per this many frames, per centity",
			generate_hash("cg_weaponVisInterval")
		},
		{
			"cg_youInKillCamSize",
			"Size of the 'you' Icon in the kill cam",
			generate_hash("cg_youInKillCamSize")
		},
		{
			"cl_anglespeedkey",
			"Multiplier for max angle speed for game pad and keyboard",
			generate_hash("cl_anglespeedkey")
		},
		{
			"cl_bypassMouseInput",
			"Bypass UI mouse input and send directly to the game",
			generate_hash("cl_bypassMouseInput")
		},
		{
			"cl_connectionAttempts",
			"Maximum number of connection attempts before aborting",
			generate_hash("cl_connectionAttempts")
		},
		{
			"cl_connectTimeout",
			"Timeout time in seconds while connecting to a server",
			generate_hash("cl_connectTimeout")
		},
		{
			"cl_demo_uploadfb",
			"Should we upload to FB",
			generate_hash("cl_demo_uploadfb")
		},
		{
			"cl_dirSelConvergenceTime",
			"Time to converge to the new direction when selecting a direction on the map.",
			generate_hash("cl_dirSelConvergenceTime")
		},
		{
			"cl_force_paused",
			"Force the client to be paused. Can't be overridden by LUA scripts, the start button, etc.",
			generate_hash("cl_force_paused")
		},
		{
			"cl_freelook",
			"Enable looking with mouse",
			generate_hash("cl_freelook")
		},
		{
			"cl_hudDrawsBehindUI",
			"Should the HUD draw when the UI is up?",
			generate_hash("cl_hudDrawsBehindUI")
		},
		{
			"cl_ingame",
			"True if the game is active",
			generate_hash("cl_ingame")
		},
		{
			"cl_inhibit_stats_upload",
			"Inhibit upload of stats during demo playback",
			generate_hash("cl_inhibit_stats_upload")
		},
		{
			"cl_lessprint",
			"Print less to the console by filtering out certain spammy channels",
			generate_hash("cl_lessprint")
		},
		{
			"cl_maxpackets",
			"Maximum number of packets sent per frame",
			generate_hash("cl_maxpackets")
		},
		{
			"cl_maxPing",
			"Maximum ping for the client",
			generate_hash("cl_maxPing")
		},
		{
			"cl_migrationTimeout",
			"Seconds to wait to hear from new host during host migration before timeout occurs",
			generate_hash("cl_migrationTimeout")
		},
		{
			"cl_modifiedDebugPlacement",
			"Modify the location of debug output (outside of safe area)",
			generate_hash("cl_modifiedDebugPlacement")
		},
		{
			"cl_motdString",
			"",
			generate_hash("cl_motdString")
		},
		{
			"cl_mouseAccel",
			"Mouse acceleration",
			generate_hash("cl_mouseAccel")
		},
		{
			"cl_noprint",
			"Print nothing to the console",
			generate_hash("cl_noprint")
		},
		{
			"cl_packetdup",
			"Enable packet duplication",
			generate_hash("cl_packetdup")
		},
		{
			"cl_pauseAudioZoneEnabled",
			"Enable the paused audio zone when the menus are up",
			generate_hash("cl_pauseAudioZoneEnabled")
		},
		{
			"cl_paused",
			"Pause the game",
			generate_hash("cl_paused")
		},
		{
			"cl_pitchspeed",
			"Max pitch speed in degrees for game pad",
			generate_hash("cl_pitchspeed")
		},
		{
			"cl_pranks",
			"pranks",
			generate_hash("cl_pranks")
		},
		{
			"cl_pushToTalk",
			"Do we have to press a button to talk",
			generate_hash("cl_pushToTalk")
		},
		{
			"cl_serverStatusResendTime",
			"Time in milliseconds to resend a server status message",
			generate_hash("cl_serverStatusResendTime")
		},
		{
			"cl_showmouserate",
			"Print mouse rate debugging information to the console",
			generate_hash("cl_showmouserate")
		},
		{
			"cl_textChatEnabled",
			"Do we want to use text chat",
			generate_hash("cl_textChatEnabled")
		},
		{
			"cl_timeout",
			"Seconds with no received packets until a timeout occurs",
			generate_hash("cl_timeout")
		},
		{
			"cl_voice",
			"Use voice communications",
			generate_hash("cl_voice")
		},
		{
			"cl_yawspeed",
			"Max yaw speed in degrees for game pad and keyboard",
			generate_hash("cl_yawspeed")
		},
		{
			"clientSideEffects",
			"Enable loading _fx.gsc files on the client",
			generate_hash("clientSideEffects")
		},
		{
			"cod_anywhere_errorMessage",
			"CoD Anywhere error message",
			generate_hash("cod_anywhere_errorMessage")
		},
		{
			"cod_anywhere_showPopup",
			"Temp Development: Should we show the CoD Anywhere popup",
			generate_hash("cod_anywhere_showPopup")
		},
		{
			"cod_anywhere_single_task_popup_text",
			"CoD Anywhere success message",
			generate_hash("cod_anywhere_single_task_popup_text")
		},
		{
			"com_animCheck",
			"Check anim tree",
			generate_hash("com_animCheck")
		},
		{
			"com_cinematicEndInWhite",
			"Set by script. True if cinematic ends with a white screen.",
			generate_hash("com_cinematicEndInWhite")
		},
		{
			"com_completionResolveCommand",
			"Command to run when the message box successfully closes",
			generate_hash("com_completionResolveCommand")
		},
		{
			"com_errorMessage",
			"Most recent error message",
			generate_hash("com_errorMessage")
		},
		{
			"com_errorResolveCommand",
			"Command to run when they close the error box",
			generate_hash("com_errorResolveCommand")
		},
		{
			"com_filter_output",
			"Use console filters for filtering output.",
			generate_hash("com_filter_output")
		},
		{
			"com_maxfps",
			"Cap frames per second",
			generate_hash("com_maxfps")
		},
		{
			"com_maxFrameTime",
			"Time slows down if a frame takes longer than this many milliseconds",
			generate_hash("com_maxFrameTime")
		},
		{
			"com_playerProfile",
			"Set to the name of the profile",
			generate_hash("com_playerProfile")
		},
		{
			"com_recommendedSet",
			"",
			generate_hash("com_recommendedSet")
		},
		{
			"commerce_dl_retry_step",
			"Step in m/s for the commerce download retry",
			generate_hash("commerce_dl_retry_step")
		},
		{
			"commerce_manifest_file_max_retry_time",
			"Max time that the commerce manifest can retry",
			generate_hash("commerce_manifest_file_max_retry_time")
		},
		{
			"commerce_manifest_file_retry_step",
			"Step in m/s for the commerce manifest retry",
			generate_hash("commerce_manifest_file_retry_step")
		},
		{
			"commerce_max_dl_retry_time",
			"Max time that the commerce download can retry",
			generate_hash("commerce_max_dl_retry_time")
		},
		{
			"commerce_max_retry_time",
			"Max time that the commerce upload can retry",
			generate_hash("commerce_max_retry_time")
		},
		{
			"commerce_retry_step",
			"Step in m/s for the commerce upload retry",
			generate_hash("commerce_retry_step")
		},
		{
			"compass",
			"Display Compass",
			generate_hash("compass")
		},
		{
			"compassClampIcons",
			"If true, friendlies and enemy pings clamp to the edge of the radar.  If false, they disappear off the edge.",
			generate_hash("compassClampIcons")
		},
		{
			"compassCoords",
			"x = North-South coord base value, \ny = East-West coord base value, \nz = scale (game units per coord unit)",
			generate_hash("compassCoords")
		},
		{
			"compassECoordCutoff",
			"Left cutoff for the scrolling east-west coords",
			generate_hash("compassECoordCutoff")
		},
		{
			"compassFriendlyHeight",
			"",
			generate_hash("compassFriendlyHeight")
		},
		{
			"compassFriendlyWidth",
			"",
			generate_hash("compassFriendlyWidth")
		},
		{
			"compassHideSansObjectivePointer",
			"Hide the compass, but leave the obective pointer visible.",
			generate_hash("compassHideSansObjectivePointer")
		},
		{
			"compassHideVehicles",
			"When enabled, disables the CG_PLAYER_COMPASS_VEHICLES ownerdraw.",
			generate_hash("compassHideVehicles")
		},
		{
			"compassMaxRange",
			"The maximum range from the player in world space that objects will be shown on the compass",
			generate_hash("compassMaxRange")
		},
		{
			"compassMinRadius",
			"The minimum radius from the center of the compass that objects will appear.",
			generate_hash("compassMinRadius")
		},
		{
			"compassMinRange",
			"The minimum range from the player in world space that objects will appear on the compass",
			generate_hash("compassMinRange")
		},
		{
			"compassObjectiveArrowHeight",
			"",
			generate_hash("compassObjectiveArrowHeight")
		},
		{
			"compassObjectiveArrowOffset",
			"The offset of the objective arrow inward from the edge of the compass map",
			generate_hash("compassObjectiveArrowOffset")
		},
		{
			"compassObjectiveArrowRotateDist",
			"Distance from the corner of the compass map at which the objective arrow rotates to 45 degrees",
			generate_hash("compassObjectiveArrowRotateDist")
		},
		{
			"compassObjectiveArrowWidth",
			"",
			generate_hash("compassObjectiveArrowWidth")
		},
		{
			"compassObjectiveDetailDist",
			"When an objective is closer than this distance (in meters), the icon will not be drawn on the tickertape.",
			generate_hash("compassObjectiveDetailDist")
		},
		{
			"compassObjectiveDrawLines",
			"Draw horizontal and vertical lines to the active target, if it is within the minimap boundries",
			generate_hash("compassObjectiveDrawLines")
		},
		{
			"compassObjectiveHeight",
			"",
			generate_hash("compassObjectiveHeight")
		},
		{
			"compassObjectiveIconHeight",
			"",
			generate_hash("compassObjectiveIconHeight")
		},
		{
			"compassObjectiveIconWidth",
			"",
			generate_hash("compassObjectiveIconWidth")
		},
		{
			"compassObjectiveMaxHeight",
			"The maximum height that an objective is considered to be on this level",
			generate_hash("compassObjectiveMaxHeight")
		},
		{
			"compassObjectiveMaxRange",
			"The maximum range at which an objective is visible on the compass",
			generate_hash("compassObjectiveMaxRange")
		},
		{
			"compassObjectiveMinAlpha",
			"The minimum alpha for an objective at the edge of the compass",
			generate_hash("compassObjectiveMinAlpha")
		},
		{
			"compassObjectiveMinDistRange",
			"The distance that objective transition effects play over, centered on compassObjectiveNearbyDist.",
			generate_hash("compassObjectiveMinDistRange")
		},
		{
			"compassObjectiveMinHeight",
			"The minimum height that an objective is considered to be on this level",
			generate_hash("compassObjectiveMinHeight")
		},
		{
			"compassObjectiveNearbyDist",
			"When an objective is closer than this distance (in meters), the icon will not be drawn on the tickertape.",
			generate_hash("compassObjectiveNearbyDist")
		},
		{
			"compassObjectiveNumRings",
			"The number of rings when a new objective appears",
			generate_hash("compassObjectiveNumRings")
		},
		{
			"compassObjectiveRingSize",
			"The maximum objective ring sige when a new objective appears on the compass",
			generate_hash("compassObjectiveRingSize")
		},
		{
			"compassObjectiveRingTime",
			"The amount of time between each ring when an objective appears",
			generate_hash("compassObjectiveRingTime")
		},
		{
			"compassObjectiveTextHeight",
			"Objective text height",
			generate_hash("compassObjectiveTextHeight")
		},
		{
			"compassObjectiveTextScale",
			"Scale to apply to hud objectives",
			generate_hash("compassObjectiveTextScale")
		},
		{
			"compassObjectiveWidth",
			"",
			generate_hash("compassObjectiveWidth")
		},
		{
			"compassObjectiveWraparoundTime",
			"How long it takes for the objective to wrap around the compass from one edge to the other",
			generate_hash("compassObjectiveWraparoundTime")
		},
		{
			"compassPlayerHeight",
			"",
			generate_hash("compassPlayerHeight")
		},
		{
			"compassPlayerWidth",
			"",
			generate_hash("compassPlayerWidth")
		},
		{
			"compassRadarLineThickness",
			"Thickness, relative to the compass size, of the radar texture that sweeps across the map",
			generate_hash("compassRadarLineThickness")
		},
		{
			"compassRadarPingFadeTime",
			"How long an enemy is visible on the compass after it is detected by radar",
			generate_hash("compassRadarPingFadeTime")
		},
		{
			"compassRotation",
			"Style of compass",
			generate_hash("compassRotation")
		},
		{
			"compassSize",
			"Scale the compass",
			generate_hash("compassSize")
		},
		{
			"compassSoundPingFadeTime",
			"The time in seconds for the sound overlay on the compass to fade",
			generate_hash("compassSoundPingFadeTime")
		},
		{
			"compassTickertapeStretch",
			"How far the tickertape should stretch from its center.",
			generate_hash("compassTickertapeStretch")
		},
		{
			"comscore_active",
			"Are we allowed to enable ComScore tracking or not",
			generate_hash("comscore_active")
		},
		{
			"con_gameMsgWindow0FadeInTime",
			"",
			generate_hash("con_gameMsgWindow0FadeInTime")
		},
		{
			"con_gameMsgWindow0FadeOutTime",
			"",
			generate_hash("con_gameMsgWindow0FadeOutTime")
		},
		{
			"con_gameMsgWindow0Filter",
			"",
			generate_hash("con_gameMsgWindow0Filter")
		},
		{
			"con_gameMsgWindow0LineCount",
			"",
			generate_hash("con_gameMsgWindow0LineCount")
		},
		{
			"con_gameMsgWindow0MsgTime",
			"",
			generate_hash("con_gameMsgWindow0MsgTime")
		},
		{
			"con_gameMsgWindow0ScrollTime",
			"",
			generate_hash("con_gameMsgWindow0ScrollTime")
		},
		{
			"con_gameMsgWindow1FadeInTime",
			"",
			generate_hash("con_gameMsgWindow1FadeInTime")
		},
		{
			"con_gameMsgWindow1FadeOutTime",
			"",
			generate_hash("con_gameMsgWindow1FadeOutTime")
		},
		{
			"con_gameMsgWindow1Filter",
			"",
			generate_hash("con_gameMsgWindow1Filter")
		},
		{
			"con_gameMsgWindow1LineCount",
			"",
			generate_hash("con_gameMsgWindow1LineCount")
		},
		{
			"con_gameMsgWindow1MsgTime",
			"",
			generate_hash("con_gameMsgWindow1MsgTime")
		},
		{
			"con_gameMsgWindow1ScrollTime",
			"",
			generate_hash("con_gameMsgWindow1ScrollTime")
		},
		{
			"con_gameMsgWindow2FadeInTime",
			"",
			generate_hash("con_gameMsgWindow2FadeInTime")
		},
		{
			"con_gameMsgWindow2FadeOutTime",
			"",
			generate_hash("con_gameMsgWindow2FadeOutTime")
		},
		{
			"con_gameMsgWindow2Filter",
			"",
			generate_hash("con_gameMsgWindow2Filter")
		},
		{
			"con_gameMsgWindow2LineCount",
			"",
			generate_hash("con_gameMsgWindow2LineCount")
		},
		{
			"con_gameMsgWindow2MsgTime",
			"",
			generate_hash("con_gameMsgWindow2MsgTime")
		},
		{
			"con_gameMsgWindow2ScrollTime",
			"",
			generate_hash("con_gameMsgWindow2ScrollTime")
		},
		{
			"con_gameMsgWindow3FadeInTime",
			"",
			generate_hash("con_gameMsgWindow3FadeInTime")
		},
		{
			"con_gameMsgWindow3FadeOutTime",
			"",
			generate_hash("con_gameMsgWindow3FadeOutTime")
		},
		{
			"con_gameMsgWindow3Filter",
			"",
			generate_hash("con_gameMsgWindow3Filter")
		},
		{
			"con_gameMsgWindow3LineCount",
			"",
			generate_hash("con_gameMsgWindow3LineCount")
		},
		{
			"con_gameMsgWindow3MsgTime",
			"",
			generate_hash("con_gameMsgWindow3MsgTime")
		},
		{
			"con_gameMsgWindow3ScrollTime",
			"",
			generate_hash("con_gameMsgWindow3ScrollTime")
		},
		{
			"con_subtitleLeading",
			"Leading for subtitles, calculated as a percentage of the font height",
			generate_hash("con_subtitleLeading")
		},
		{
			"con_typewriterColorGlowCheckpoint",
			"",
			generate_hash("con_typewriterColorGlowCheckpoint")
		},
		{
			"con_typewriterColorGlowCompleted",
			"",
			generate_hash("con_typewriterColorGlowCompleted")
		},
		{
			"con_typewriterColorGlowFailed",
			"",
			generate_hash("con_typewriterColorGlowFailed")
		},
		{
			"con_typewriterColorGlowUpdated",
			"",
			generate_hash("con_typewriterColorGlowUpdated")
		},
		{
			"con_typewriterColorInteriorCheckpoint",
			"",
			generate_hash("con_typewriterColorInteriorCheckpoint")
		},
		{
			"con_typewriterColorInteriorCompleted",
			"",
			generate_hash("con_typewriterColorInteriorCompleted")
		},
		{
			"con_typewriterColorInteriorFailed",
			"",
			generate_hash("con_typewriterColorInteriorFailed")
		},
		{
			"con_typewriterColorInteriorUpdated",
			"",
			generate_hash("con_typewriterColorInteriorUpdated")
		},
		{
			"con_typewriterDecayDuration",
			"Time (in milliseconds) to spend disolving the line away.",
			generate_hash("con_typewriterDecayDuration")
		},
		{
			"con_typewriterDecayStartTime",
			"Time (in milliseconds) to spend between the build and disolve phases.",
			generate_hash("con_typewriterDecayStartTime")
		},
		{
			"con_typewriterPrintSpeed",
			"Time (in milliseconds) to print each letter in the line.",
			generate_hash("con_typewriterPrintSpeed")
		},
		{
			"counterDownloadInterval",
			"Number of minutes before all the global counters are uploaded",
			generate_hash("counterDownloadInterval")
		},
		{
			"counterUploadInterval",
			"Number of minutes before all the global counters are uploaded",
			generate_hash("counterUploadInterval")
		},
		{
			"cpu_speed_12players",
			"12 player sys_configureGHz req'd",
			generate_hash("cpu_speed_12players")
		},
		{
			"cpu_speed_18players",
			"18 player sys_configureGHz req'd",
			generate_hash("cpu_speed_18players")
		},
		{
			"cpu_speed_8players",
			"8 player sys_configureGHz req'd",
			generate_hash("cpu_speed_8players")
		},
		{
			"cSplineDebugRender",
			"Debug Render the csplines.",
			generate_hash("cSplineDebugRender")
		},
		{
			"cSplineDebugRenderCorridor",
			"Debug Render the cspline corridor.",
			generate_hash("cSplineDebugRenderCorridor")
		},
		{
			"cSplineDebugRenderData",
			"Debug Render the cspline data.",
			generate_hash("cSplineDebugRenderData")
		},
		{
			"cSplineDebugRenderSplineId",
			"Select a cspline - 0 for all.",
			generate_hash("cSplineDebugRenderSplineId")
		},
		{
			"dailychallenge_killswitch",
			"daily challenge killswitch - int with bits used to flag individual daily challenges as enabled",
			generate_hash("dailychallenge_killswitch")
		},
		{
			"dailychallenge_killswitch2",
			"daily challenge killswitch2 - int with bits used to flag 2nd set of individual daily challenges as enabled",
			generate_hash("dailychallenge_killswitch2")
		},
		{
			"dailychallenge_period",
			"daily challenge period - utc value for a day",
			generate_hash("dailychallenge_period")
		},
		{
			"data_validation_allow_drop",
			"",
			generate_hash("data_validation_allow_drop")
		},
		{
			"dc_lobbymerge",
			"Allows lobby merging across data centres",
			generate_hash("dc_lobbymerge")
		},
		{
			"dcacheSimulateNoHDD",
			"When turned on, simulate no HDD for caching.",
			generate_hash("dcacheSimulateNoHDD")
		},
		{
			"dcacheThrottleEnabled",
			"Enable or disable dcache upload throttling.",
			generate_hash("dcacheThrottleEnabled")
		},
		{
			"dcacheThrottleKBytesPerSec",
			"Dcache upload throttle limit in K Bytes per second.",
			generate_hash("dcacheThrottleKBytesPerSec")
		},
		{
			"dedicated_dhclient",
			"True if we're a client playing on a DH server",
			generate_hash("dedicated_dhclient")
		},
		{
			"demonwareConsideredConnectedTime",
			"Number of milliseconds after being disconnected from demonware before considering shutting down.",
			generate_hash("demonwareConsideredConnectedTime")
		},
		{
			"developer",
			"Enable development options",
			generate_hash("developer")
		},
		{
			"didyouknow",
			"",
			generate_hash("didyouknow")
		},
		{
			"discard_playerstats_on_suspend",
			"Forces stats discard on suspend",
			generate_hash("discard_playerstats_on_suspend")
		},
		{
			"drawEntityCount",
			"Enable entity count drawing",
			generate_hash("drawEntityCount")
		},
		{
			"drawEntityCountPos",
			"Where to draw the entity count graph",
			generate_hash("drawEntityCountPos")
		},
		{
			"drawEntityCountSize",
			"undefined",
			generate_hash("drawEntityCountSize")
		},
		{
			"drawKillcamData",
			"Enable drawing server killcam data",
			generate_hash("drawKillcamData")
		},
		{
			"drawKillcamDataPos",
			"Where to draw the server killcam graph",
			generate_hash("drawKillcamDataPos")
		},
		{
			"drawKillcamDataSize",
			"How big to draw the killcam data graph",
			generate_hash("drawKillcamDataSize")
		},
		{
			"drawServerBandwidth",
			"Enable drawing server bandwidth",
			generate_hash("drawServerBandwidth")
		},
		{
			"drawServerBandwidthPos",
			"Where to draw the server bandwidth graph",
			generate_hash("drawServerBandwidthPos")
		},
		{
			"ds_dcid",
			"optional datacenter id - from playlist",
			generate_hash("ds_dcid")
		},
		{
			"ds_dcid_override",
			"force datacenter id",
			generate_hash("ds_dcid_override")
		},
		{
			"ds_info",
			"ds info string",
			generate_hash("ds_info")
		},
		{
			"ds_info_enable",
			"Enable ds info string",
			generate_hash("ds_info_enable")
		},
		{
			"ds_introRequestTimeout",
			"ds intro request timeout (ms)",
			generate_hash("ds_introRequestTimeout")
		},
		{
			"ds_keepaliveInterval",
			"ds keepalive interval (ms)",
			generate_hash("ds_keepaliveInterval")
		},
		{
			"ds_keepaliveTimeout",
			"ds keepalive timeout (ms)",
			generate_hash("ds_keepaliveTimeout")
		},
		{
			"ds_pingclient_max_reping_distance",
			"don't re-ping a datacenter if it's further away than this (miles)",
			generate_hash("ds_pingclient_max_reping_distance")
		},
		{
			"ds_pingclient_max_repings",
			"max # of times to re-ping a datacenter",
			generate_hash("ds_pingclient_max_repings")
		},
		{
			"ds_pingclient_maxpings",
			"max pings to send per datacenter",
			generate_hash("ds_pingclient_maxpings")
		},
		{
			"ds_pingclient_maxpings_per_tick",
			"max new pings each tick",
			generate_hash("ds_pingclient_maxpings_per_tick")
		},
		{
			"ds_pingclient_min_reping_delay",
			"min msec delay between re-pings",
			generate_hash("ds_pingclient_min_reping_delay")
		},
		{
			"ds_pingclient_min_reping_latency",
			"don't re-ping a datacenter if latency is less than this",
			generate_hash("ds_pingclient_min_reping_latency")
		},
		{
			"ds_pingclient_minpings",
			"min responses required per datacenter",
			generate_hash("ds_pingclient_minpings")
		},
		{
			"ds_pingclient_odsf",
			"does dsping set odsf flag",
			generate_hash("ds_pingclient_odsf")
		},
		{
			"dsping_dc_0",
			"",
			generate_hash("dsping_dc_0")
		},
		{
			"dsping_dc_1",
			"",
			generate_hash("dsping_dc_1")
		},
		{
			"dsping_dc_10",
			"",
			generate_hash("dsping_dc_10")
		},
		{
			"dsping_dc_11",
			"",
			generate_hash("dsping_dc_11")
		},
		{
			"dsping_dc_12",
			"",
			generate_hash("dsping_dc_12")
		},
		{
			"dsping_dc_13",
			"",
			generate_hash("dsping_dc_13")
		},
		{
			"dsping_dc_14",
			"",
			generate_hash("dsping_dc_14")
		},
		{
			"dsping_dc_15",
			"",
			generate_hash("dsping_dc_15")
		},
		{
			"dsping_dc_16",
			"",
			generate_hash("dsping_dc_16")
		},
		{
			"dsping_dc_17",
			"",
			generate_hash("dsping_dc_17")
		},
		{
			"dsping_dc_18",
			"",
			generate_hash("dsping_dc_18")
		},
		{
			"dsping_dc_19",
			"",
			generate_hash("dsping_dc_19")
		},
		{
			"dsping_dc_2",
			"",
			generate_hash("dsping_dc_2")
		},
		{
			"dsping_dc_20",
			"",
			generate_hash("dsping_dc_20")
		},
		{
			"dsping_dc_21",
			"",
			generate_hash("dsping_dc_21")
		},
		{
			"dsping_dc_22",
			"",
			generate_hash("dsping_dc_22")
		},
		{
			"dsping_dc_23",
			"",
			generate_hash("dsping_dc_23")
		},
		{
			"dsping_dc_24",
			"",
			generate_hash("dsping_dc_24")
		},
		{
			"dsping_dc_25",
			"",
			generate_hash("dsping_dc_25")
		},
		{
			"dsping_dc_26",
			"",
			generate_hash("dsping_dc_26")
		},
		{
			"dsping_dc_27",
			"",
			generate_hash("dsping_dc_27")
		},
		{
			"dsping_dc_28",
			"",
			generate_hash("dsping_dc_28")
		},
		{
			"dsping_dc_29",
			"",
			generate_hash("dsping_dc_29")
		},
		{
			"dsping_dc_3",
			"",
			generate_hash("dsping_dc_3")
		},
		{
			"dsping_dc_30",
			"",
			generate_hash("dsping_dc_30")
		},
		{
			"dsping_dc_31",
			"",
			generate_hash("dsping_dc_31")
		},
		{
			"dsping_dc_32",
			"",
			generate_hash("dsping_dc_32")
		},
		{
			"dsping_dc_33",
			"",
			generate_hash("dsping_dc_33")
		},
		{
			"dsping_dc_34",
			"",
			generate_hash("dsping_dc_34")
		},
		{
			"dsping_dc_35",
			"",
			generate_hash("dsping_dc_35")
		},
		{
			"dsping_dc_36",
			"",
			generate_hash("dsping_dc_36")
		},
		{
			"dsping_dc_37",
			"",
			generate_hash("dsping_dc_37")
		},
		{
			"dsping_dc_38",
			"",
			generate_hash("dsping_dc_38")
		},
		{
			"dsping_dc_39",
			"",
			generate_hash("dsping_dc_39")
		},
		{
			"dsping_dc_4",
			"",
			generate_hash("dsping_dc_4")
		},
		{
			"dsping_dc_40",
			"",
			generate_hash("dsping_dc_40")
		},
		{
			"dsping_dc_41",
			"",
			generate_hash("dsping_dc_41")
		},
		{
			"dsping_dc_42",
			"",
			generate_hash("dsping_dc_42")
		},
		{
			"dsping_dc_43",
			"",
			generate_hash("dsping_dc_43")
		},
		{
			"dsping_dc_44",
			"",
			generate_hash("dsping_dc_44")
		},
		{
			"dsping_dc_45",
			"",
			generate_hash("dsping_dc_45")
		},
		{
			"dsping_dc_46",
			"",
			generate_hash("dsping_dc_46")
		},
		{
			"dsping_dc_47",
			"",
			generate_hash("dsping_dc_47")
		},
		{
			"dsping_dc_48",
			"",
			generate_hash("dsping_dc_48")
		},
		{
			"dsping_dc_49",
			"",
			generate_hash("dsping_dc_49")
		},
		{
			"dsping_dc_5",
			"",
			generate_hash("dsping_dc_5")
		},
		{
			"dsping_dc_50",
			"",
			generate_hash("dsping_dc_50")
		},
		{
			"dsping_dc_51",
			"",
			generate_hash("dsping_dc_51")
		},
		{
			"dsping_dc_52",
			"",
			generate_hash("dsping_dc_52")
		},
		{
			"dsping_dc_53",
			"",
			generate_hash("dsping_dc_53")
		},
		{
			"dsping_dc_54",
			"",
			generate_hash("dsping_dc_54")
		},
		{
			"dsping_dc_55",
			"",
			generate_hash("dsping_dc_55")
		},
		{
			"dsping_dc_56",
			"",
			generate_hash("dsping_dc_56")
		},
		{
			"dsping_dc_57",
			"",
			generate_hash("dsping_dc_57")
		},
		{
			"dsping_dc_58",
			"",
			generate_hash("dsping_dc_58")
		},
		{
			"dsping_dc_59",
			"",
			generate_hash("dsping_dc_59")
		},
		{
			"dsping_dc_6",
			"",
			generate_hash("dsping_dc_6")
		},
		{
			"dsping_dc_60",
			"",
			generate_hash("dsping_dc_60")
		},
		{
			"dsping_dc_61",
			"",
			generate_hash("dsping_dc_61")
		},
		{
			"dsping_dc_62",
			"",
			generate_hash("dsping_dc_62")
		},
		{
			"dsping_dc_63",
			"",
			generate_hash("dsping_dc_63")
		},
		{
			"dsping_dc_7",
			"",
			generate_hash("dsping_dc_7")
		},
		{
			"dsping_dc_8",
			"",
			generate_hash("dsping_dc_8")
		},
		{
			"dsping_dc_9",
			"",
			generate_hash("dsping_dc_9")
		},
		{
			"dvl",
			"Enables the data validation system. Only available in non-retail builds.",
			generate_hash("dvl")
		},
		{
			"dw_addrHandleTimeout",
			"Delay before destroying an addrHandle after the connection is lost\n",
			generate_hash("dw_addrHandleTimeout")
		},
		{
			"dw_leaderboard_write_active",
			"Are leaderboard writes enabled",
			generate_hash("dw_leaderboard_write_active")
		},
		{
			"dw_presence_active",
			"Is the demonware presence system enabled",
			generate_hash("dw_presence_active")
		},
		{
			"dw_presence_coop_join_active",
			"Do we allow players to join on presence for private coop matches (post session to demonware",
			generate_hash("dw_presence_coop_join_active")
		},
		{
			"dw_presence_get_delay",
			"Number of milliseconds to wait after booting the game to fetch demonware presence",
			generate_hash("dw_presence_get_delay")
		},
		{
			"dw_presence_get_rate",
			"Number of milliseconds to wait between sending presence state to demonware",
			generate_hash("dw_presence_get_rate")
		},
		{
			"dw_presence_put_delay",
			"Number of milliseconds to wait in a presence state before sending to demonware",
			generate_hash("dw_presence_put_delay")
		},
		{
			"dw_presence_put_rate",
			"Number of milliseconds to wait between sending presence state to demonware",
			generate_hash("dw_presence_put_rate")
		},
		{
			"dw_region_lookup_timeout",
			"Timeout (in MS) after which we will accept not having found a region code and use the default",
			generate_hash("dw_region_lookup_timeout")
		},
		{
			"dw_shared_presence_active",
			"Is the demonware shared presence system enabled",
			generate_hash("dw_shared_presence_active")
		},
		{
			"dw_shared_presence_get_delay",
			"Number of milliseconds to wait after booting the game to fetch demonware presence",
			generate_hash("dw_shared_presence_get_delay")
		},
		{
			"dw_shared_presence_get_rate",
			"Number of milliseconds to wait between sending presence state to demonware",
			generate_hash("dw_shared_presence_get_rate")
		},
		{
			"dw_shared_presence_put_delay",
			"Number of milliseconds to wait in a shared presence state before sending to demonware",
			generate_hash("dw_shared_presence_put_delay")
		},
		{
			"dw_shared_presence_put_rate",
			"Number of milliseconds to wait between sending presence state to demonware",
			generate_hash("dw_shared_presence_put_rate")
		},
		{
			"dwBandwidthTestTaskTimeout",
			"default timeout for the bandwidth test task (in ms). 0 means no timeout",
			generate_hash("dwBandwidthTestTaskTimeout")
		},
		{
			"dynEnt_active",
			"Disable/enable dynent reactions",
			generate_hash("dynEnt_active")
		},
		{
			"dynEnt_playerWakeUpRadius",
			"Determines threshold distance from player within which all dynents are woken up.",
			generate_hash("dynEnt_playerWakeUpRadius")
		},
		{
			"dynEnt_playerWakeUpZOffset",
			"Determines vertical distance from player's feet from which wake up sphere is centered.",
			generate_hash("dynEnt_playerWakeUpZOffset")
		},
		{
			"elite_clan_active",
			"Are we allowed to show Elite Clans or not",
			generate_hash("elite_clan_active")
		},
		{
			"elite_clan_cool_off_time",
			"Cool off time between calls to fetch the elite clan",
			generate_hash("elite_clan_cool_off_time")
		},
		{
			"elite_clan_delay",
			"Delay before the bdTeams calls start to Demonware. -1 means On-Demand and it will wait until the 'starteliteclan' menu call",
			generate_hash("elite_clan_delay")
		},
		{
			"elite_clan_division_icon_active",
			"Are we allowed to show Elite Clan division icon or not",
			generate_hash("elite_clan_division_icon_active")
		},
		{
			"elite_clan_get_blob_profile_max_retry_time",
			"Max time that the Elite Clan get private profile can retry",
			generate_hash("elite_clan_get_blob_profile_max_retry_time")
		},
		{
			"elite_clan_get_blob_profile_retry_step",
			"Step in m/s for the Elite Clan get private profile retry",
			generate_hash("elite_clan_get_blob_profile_retry_step")
		},
		{
			"elite_clan_get_clan_max_retry_time",
			"Max time that the Elite Clan get clan can retry",
			generate_hash("elite_clan_get_clan_max_retry_time")
		},
		{
			"elite_clan_get_clan_retry_step",
			"Step in m/s for the Elite Clan get clan retry",
			generate_hash("elite_clan_get_clan_retry_step")
		},
		{
			"elite_clan_get_members_max_retry_time",
			"Max time that the Elite Clan get members can retry",
			generate_hash("elite_clan_get_members_max_retry_time")
		},
		{
			"elite_clan_get_members_retry_step",
			"Step in m/s for the Elite Clan get members retry",
			generate_hash("elite_clan_get_members_retry_step")
		},
		{
			"elite_clan_get_private_member_profile_max_retry_time",
			"Max time that the Elite Clan get private profile can retry",
			generate_hash("elite_clan_get_private_member_profile_max_retry_time")
		},
		{
			"elite_clan_get_private_member_profile_retry_step",
			"Step in m/s for the Elite Clan get private profile retry",
			generate_hash("elite_clan_get_private_member_profile_retry_step")
		},
		{
			"elite_clan_get_public_profile_max_retry_time",
			"Max time that the Elite Clan get public profile can retry",
			generate_hash("elite_clan_get_public_profile_max_retry_time")
		},
		{
			"elite_clan_get_public_profile_retry_step",
			"Step in m/s for the Elite Clan get public profile retry",
			generate_hash("elite_clan_get_public_profile_retry_step")
		},
		{
			"elite_clan_get_team_stats_max_retry_time",
			"Max time that the Elite Clan get team stats can retry",
			generate_hash("elite_clan_get_team_stats_max_retry_time")
		},
		{
			"elite_clan_get_team_stats_retry_step",
			"Step in m/s for the Elite Clan get team stats retry",
			generate_hash("elite_clan_get_team_stats_retry_step")
		},
		{
			"elite_clan_motd_throttle_time",
			"Throttle time between motd update calls",
			generate_hash("elite_clan_motd_throttle_time")
		},
		{
			"elite_clan_remote_view_active",
			"Are we allowed to view the clans for remote players",
			generate_hash("elite_clan_remote_view_active")
		},
		{
			"elite_clan_remote_view_max_retry_time",
			"Max time that the Elite Clan remote viewing can retry",
			generate_hash("elite_clan_remote_view_max_retry_time")
		},
		{
			"elite_clan_remote_view_retry_step",
			"Step in m/s for the retry for viewing a remote Elite Clan",
			generate_hash("elite_clan_remote_view_retry_step")
		},
		{
			"elite_clan_send_message_to_members_max_retry_time",
			"Max time that the Elite Clan send message to members can retry",
			generate_hash("elite_clan_send_message_to_members_max_retry_time")
		},
		{
			"elite_clan_send_message_to_members_retry_step",
			"Step in m/s for the Elite Clan send message to members retry",
			generate_hash("elite_clan_send_message_to_members_retry_step")
		},
		{
			"elite_clan_set_private_member_profile_max_retry_time",
			"Max time that the Elite Clan set private member profile can retry",
			generate_hash("elite_clan_set_private_member_profile_max_retry_time")
		},
		{
			"elite_clan_set_private_member_profile_retry_step",
			"Step in m/s for the Elite Clan set private member profile retry",
			generate_hash("elite_clan_set_private_member_profile_retry_step")
		},
		{
			"elite_clan_single_task_popup_text",
			"String to be displayed on popup when a single task is being performed",
			generate_hash("elite_clan_single_task_popup_text")
		},
		{
			"elite_clan_using_title",
			"Stores whether the Elite Clan title is in use by the user",
			generate_hash("elite_clan_using_title")
		},
		{
			"emblems_active",
			"Are we allowed to enable Emblems or not",
			generate_hash("emblems_active")
		},
		{
			"enable_recordRecentActivity",
			"records the timestamp of when the player was recently active to the tracker leaderboards",
			generate_hash("enable_recordRecentActivity")
		},
		{
			"enableReportingRegisteredParties",
			"If true then party membership data and host status will be reported in matchdata blob.",
			generate_hash("enableReportingRegisteredParties")
		},
		{
			"entitlements_active",
			"Are we allowed to show Entitlements or not",
			generate_hash("entitlements_active")
		},
		{
			"entitlements_config_file_max_retry_time",
			"Max time that the Entitlements config file read can retry",
			generate_hash("entitlements_config_file_max_retry_time")
		},
		{
			"entitlements_config_file_retry_step",
			"Step in m/s for the Entitlements config file read retry",
			generate_hash("entitlements_config_file_retry_step")
		},
		{
			"entitlements_cool_off_time",
			"Cool off time between calls to fetch the elite clan",
			generate_hash("entitlements_cool_off_time")
		},
		{
			"entitlements_delay",
			"Delay before the entitlement calls start to Demonware. -1 means On-Demand and it will wait until the 'startentitlements' menu call",
			generate_hash("entitlements_delay")
		},
		{
			"entitlements_key_archive_max_retry_time",
			"Max time that the Entitlements key archive read can retry",
			generate_hash("entitlements_key_archive_max_retry_time")
		},
		{
			"entitlements_key_archive_retry_step",
			"Step in m/s for the Entitlements key archive read retry",
			generate_hash("entitlements_key_archive_retry_step")
		},
		{
			"entitlementSystemOk",
			"Set by the game to inform that the entitlement system is initialised",
			generate_hash("entitlementSystemOk")
		},
		{
			"facebook_active",
			"Are we allowed to show Facebook or not",
			generate_hash("facebook_active")
		},
		{
			"facebook_delay",
			"Delay before the Facebook calls start to Demonware. -1 means On-Demand and it will wait until the 'startfacebook' menu call",
			generate_hash("facebook_delay")
		},
		{
			"facebook_friends_active",
			"Are we allowed to show Facebook Friends or not",
			generate_hash("facebook_friends_active")
		},
		{
			"facebook_friends_max_retry_time",
			"Max time that the Facebook friends read can retry",
			generate_hash("facebook_friends_max_retry_time")
		},
		{
			"facebook_friends_refresh_time",
			"Time in seconds between Facebook friend refreshes",
			generate_hash("facebook_friends_refresh_time")
		},
		{
			"facebook_friends_retry_step",
			"Step in m/s for the Facebook friends read retry",
			generate_hash("facebook_friends_retry_step")
		},
		{
			"facebook_friends_showing_count",
			"Contains how many facebook friends are being shown in the UI.",
			generate_hash("facebook_friends_showing_count")
		},
		{
			"facebook_friends_throttle_time",
			"Throttle time between Facebook friend pages",
			generate_hash("facebook_friends_throttle_time")
		},
		{
			"facebook_max_retry_time",
			"Max time that the Facebook authentication can retry",
			generate_hash("facebook_max_retry_time")
		},
		{
			"facebook_password",
			"Facebook Password",
			generate_hash("facebook_password")
		},
		{
			"facebook_password_asterisk",
			"Facebook Password (Asterisk Version)",
			generate_hash("facebook_password_asterisk")
		},
		{
			"facebook_popup_text",
			"Facebook Popup Text",
			generate_hash("facebook_popup_text")
		},
		{
			"facebook_retry_step",
			"Step in m/s for the Facebook authentication retry",
			generate_hash("facebook_retry_step")
		},
		{
			"facebook_upload_photo_active",
			"Are we allowed to Upload Photos to  Facebook or not",
			generate_hash("facebook_upload_photo_active")
		},
		{
			"facebook_upload_video_active",
			"Are we allowed to Upload Videos to Facebook or not",
			generate_hash("facebook_upload_video_active")
		},
		{
			"facebook_username",
			"Facebook Username",
			generate_hash("facebook_username")
		},
		{
			"fixedtime",
			"Use a fixed time rate for each frame",
			generate_hash("fixedtime")
		},
		{
			"FoFIconMaxSize",
			"Maximum size a Friend-or-Foe icon should ever grow to.",
			generate_hash("FoFIconMaxSize")
		},
		{
			"FoFIconMinSize",
			"Minimum size a Friend-or-Foe icon should ever shrink to.",
			generate_hash("FoFIconMinSize")
		},
		{
			"FoFIconScale",
			"Base scale of Friend-or-Foe icons.",
			generate_hash("FoFIconScale")
		},
		{
			"FoFIconSpawnTimeDelay",
			"How long to wait, after spawning, before showing the Friend-or-Foe icon on a player.",
			generate_hash("FoFIconSpawnTimeDelay")
		},
		{
			"FoFIconSpawnTimeFade",
			"Length of the Friend-or-Foe icons' fade-ins.",
			generate_hash("FoFIconSpawnTimeFade")
		},
		{
			"friendsCacheSteamFriends",
			"Use cache of steam friends before querying steam api",
			generate_hash("friendsCacheSteamFriends")
		},
		{
			"friendsMaxSteamLookupsPerFrame",
			"Number of steam friends to query steam status per frame when doing a refresh.\n",
			generate_hash("friendsMaxSteamLookupsPerFrame")
		},
		{
			"friendsWidgetMinimumRefreshTimer",
			"Minimum delay before refreshing friends data if you aren't on the friends screen\n",
			generate_hash("friendsWidgetMinimumRefreshTimer")
		},
		{
			"fs_basegame",
			"Base game name",
			generate_hash("fs_basegame")
		},
		{
			"fs_basepath",
			"Base game path",
			generate_hash("fs_basepath")
		},
		{
			"fs_basepath_output",
			"Base game path",
			generate_hash("fs_basepath_output")
		},
		{
			"fs_cdpath",
			"CD path",
			generate_hash("fs_cdpath")
		},
		{
			"fs_copyfiles",
			"Copy all used files to another location",
			generate_hash("fs_copyfiles")
		},
		{
			"fs_debug",
			"Enable file system debugging information",
			generate_hash("fs_debug")
		},
		{
			"fs_game",
			"Game data directory. Must be \"\" or a sub directory of 'mods/'.",
			generate_hash("fs_game")
		},
		{
			"fs_homepath",
			"Game home path",
			generate_hash("fs_homepath")
		},
		{
			"fs_ignoreLocalized",
			"Ignore localized assets",
			generate_hash("fs_ignoreLocalized")
		},
		{
			"fx_alphaThreshold",
			"Don't draw billboard sprites, oriented sprites or tails with alpha below this threshold (0-256).",
			generate_hash("fx_alphaThreshold")
		},
		{
			"fx_cast_shadow",
			"Enable transparency shadow mapping from script",
			generate_hash("fx_cast_shadow")
		},
		{
			"fx_count",
			"Debug effects count",
			generate_hash("fx_count")
		},
		{
			"fx_cull_elem_draw",
			"Culls effect elems for drawing",
			generate_hash("fx_cull_elem_draw")
		},
		{
			"fx_cull_elem_draw_flicker",
			"Flicker DPVS culled effect elems",
			generate_hash("fx_cull_elem_draw_flicker")
		},
		{
			"fx_cull_elem_spawn",
			"Culls effect elems for spawning",
			generate_hash("fx_cull_elem_spawn")
		},
		{
			"fx_debugBolt",
			"Debug effects bolt",
			generate_hash("fx_debugBolt")
		},
		{
			"fx_deferelem",
			"Toggles deferred processing of elements instead of effects",
			generate_hash("fx_deferelem")
		},
		{
			"fx_dpvs_cull_elem_draw",
			"Culls effect elems for drawing using DPVS(2: ignore per-effect portal culling flag)",
			generate_hash("fx_dpvs_cull_elem_draw")
		},
		{
			"fx_draw",
			"",
			generate_hash("fx_draw")
		},
		{
			"fx_draw_omniLight",
			"",
			generate_hash("fx_draw_omniLight")
		},
		{
			"fx_draw_simd",
			"Draw effects using SIMD / Vector code.",
			generate_hash("fx_draw_simd")
		},
		{
			"fx_draw_spotLight",
			"",
			generate_hash("fx_draw_spotLight")
		},
		{
			"fx_drawClouds",
			"Toggles the drawing of particle clouds",
			generate_hash("fx_drawClouds")
		},
		{
			"fx_enable",
			"Toggles all effects processing",
			generate_hash("fx_enable")
		},
		{
			"fx_flare",
			"Toggles fx flare",
			generate_hash("fx_flare")
		},
		{
			"fx_freeze",
			"Freeze effects",
			generate_hash("fx_freeze")
		},
		{
			"fx_killEffectOnRewind",
			"Causes effects that have been marked for a soft kill (fade out) to be killed immediately on a rewind.",
			generate_hash("fx_killEffectOnRewind")
		},
		{
			"fx_lightGridSampleOffset",
			"the length of effect sample's offset along X Axis",
			generate_hash("fx_lightGridSampleOffset")
		},
		{
			"fx_mark_profile",
			"Turn on FX profiling for marks (specify which local client, with '1' being the first.)",
			generate_hash("fx_mark_profile")
		},
		{
			"fx_marks",
			"Toggles whether bullet hits leave marks",
			generate_hash("fx_marks")
		},
		{
			"fx_marks_ents",
			"Toggles whether bullet hits leave marks",
			generate_hash("fx_marks_ents")
		},
		{
			"fx_marks_nearlimit",
			"Sets limit of number of decals that can exist at the same location (0 for unlimited)",
			generate_hash("fx_marks_nearlimit")
		},
		{
			"fx_marks_smodels",
			"Toggles whether bullet hits leave marks",
			generate_hash("fx_marks_smodels")
		},
		{
			"fx_physicsImpactVelocityThreshold",
			"Set the min normal velocity threshold in order for model physics fx to generate child impact effects.",
			generate_hash("fx_physicsImpactVelocityThreshold")
		},
		{
			"fx_profile",
			"Turn on FX profiling (specify which local client, with '1' being the first.)",
			generate_hash("fx_profile")
		},
		{
			"fx_profileFilter",
			"Only show effects with this as a substring in FX profile",
			generate_hash("fx_profileFilter")
		},
		{
			"fx_profileFilterElemCountZero",
			"Do not include FX that have a zero element count",
			generate_hash("fx_profileFilterElemCountZero")
		},
		{
			"fx_profileSkip",
			"Skip the first n lines in FX profile (to see ones off bottom of screen)",
			generate_hash("fx_profileSkip")
		},
		{
			"fx_profileSort",
			"Choose sort criteria for FX profiling",
			generate_hash("fx_profileSort")
		},
		{
			"fx_showLightGridSampleOffset",
			"show light grid sample offset in CreateFX mode",
			generate_hash("fx_showLightGridSampleOffset")
		},
		{
			"fx_visMinTraceDist",
			"Minimum visibility trace size",
			generate_hash("fx_visMinTraceDist")
		},
		{
			"g_ai",
			"Enable AI.",
			generate_hash("g_ai")
		},
		{
			"g_allowVote",
			"Enable voting on this server",
			generate_hash("g_allowVote")
		},
		{
			"g_atmosFogDistanceScaleReadOnly",
			"scale applied to scene distance used for atmospheric fog calculation",
			generate_hash("g_atmosFogDistanceScaleReadOnly")
		},
		{
			"g_atmosFogEnabledReadOnly",
			"use atmospheric fog",
			generate_hash("g_atmosFogEnabledReadOnly")
		},
		{
			"g_atmosFogExtinctionStrengthReadOnly",
			"scale out scatter contribution of atmospheric fog",
			generate_hash("g_atmosFogExtinctionStrengthReadOnly")
		},
		{
			"g_atmosFogHalfPlaneDistanceReadOnly",
			"distance at which atmospheric fog contributes half the pixels color",
			generate_hash("g_atmosFogHalfPlaneDistanceReadOnly")
		},
		{
			"g_atmosFogHazeSpreadReadOnly",
			"directionality of haze (1ReadOnly = all forward scatter, 0ReadOnly = all back scatter)",
			generate_hash("g_atmosFogHazeSpreadReadOnly")
		},
		{
			"g_atmosFogHazeStrengthReadOnly",
			"portion of atmospheric fog density that is haze (0ReadOnly = all fog, 1ReadOnly = all haze)",
			generate_hash("g_atmosFogHazeStrengthReadOnly")
		},
		{
			"g_atmosFogHeightFogBaseHeightReadOnly",
			"height fog is full density at this world height and below",
			generate_hash("g_atmosFogHeightFogBaseHeightReadOnly")
		},
		{
			"g_atmosFogHeightFogEnabledReadOnly",
			"use height for atmospheric fog",
			generate_hash("g_atmosFogHeightFogEnabledReadOnly")
		},
		{
			"g_atmosFogHeightFogHalfPlaneDistanceReadOnly",
			"at this distance above g_atmosFogHeightFogBaseHeight, height fog density is half",
			generate_hash("g_atmosFogHeightFogHalfPlaneDistanceReadOnly")
		},
		{
			"g_atmosFogInScatterStrengthReadOnly",
			"scale in scatter contribution of atmospheric fog",
			generate_hash("g_atmosFogInScatterStrengthReadOnly")
		},
		{
			"g_atmosFogSkyAngularFalloffEnabledReadOnly",
			"use angular sky falloff for atmospheric fog",
			generate_hash("g_atmosFogSkyAngularFalloffEnabledReadOnly")
		},
		{
			"g_atmosFogSkyDistanceReadOnly",
			"distance used for sky box when applying atmospheric fog",
			generate_hash("g_atmosFogSkyDistanceReadOnly")
		},
		{
			"g_atmosFogSkyFalloffAngleRangeReadOnly",
			"sky fog angular falloff angle range sky fog falls off over this range from the start angle",
			generate_hash("g_atmosFogSkyFalloffAngleRangeReadOnly")
		},
		{
			"g_atmosFogSkyFalloffStartAngleReadOnly",
			"sky fog angular falloff start angle (full strength fog at this angle)",
			generate_hash("g_atmosFogSkyFalloffStartAngleReadOnly")
		},
		{
			"g_atmosFogStartDistanceReadOnly",
			"distance from camera at which fog contribution begins",
			generate_hash("g_atmosFogStartDistanceReadOnly")
		},
		{
			"g_atmosFogSunDirectionReadOnly",
			"sun direction used when calculating atmospheric fog",
			generate_hash("g_atmosFogSunDirectionReadOnly")
		},
		{
			"g_banIPs",
			"IP addresses to ban from playing",
			generate_hash("g_banIPs")
		},
		{
			"g_clonePlayerMaxVelocity",
			"Maximum velocity in each axis of a cloned player\n(for death animations)",
			generate_hash("g_clonePlayerMaxVelocity")
		},
		{
			"g_deadChat",
			"Allow dead players to chat with living players",
			generate_hash("g_deadChat")
		},
		{
			"g_dropForwardSpeed",
			"Forward speed of a dropped item",
			generate_hash("g_dropForwardSpeed")
		},
		{
			"g_dropHorzSpeedRand",
			"Random component of the initial horizontal speed of a dropped item",
			generate_hash("g_dropHorzSpeedRand")
		},
		{
			"g_dropUpSpeedBase",
			"Base component of the initial vertical speed of a dropped item",
			generate_hash("g_dropUpSpeedBase")
		},
		{
			"g_dropUpSpeedRand",
			"Random component of the initial vertical speed of a dropped item",
			generate_hash("g_dropUpSpeedRand")
		},
		{
			"g_earthquakeEnable",
			"Enable camera shake",
			generate_hash("g_earthquakeEnable")
		},
		{
			"g_fogColorIntensityReadOnly",
			"HDR fog color intensity that was set in the most recent call to \"setexpfog\"",
			generate_hash("g_fogColorIntensityReadOnly")
		},
		{
			"g_fogColorReadOnly",
			"Fog color that was set in the most recent call to \"setexpfog\"",
			generate_hash("g_fogColorReadOnly")
		},
		{
			"g_fogHalfDistReadOnly",
			"",
			generate_hash("g_fogHalfDistReadOnly")
		},
		{
			"g_fogMaxOpacityReadOnly",
			"Fog max opacity that was set in the most recent call to \"setexpfog\"",
			generate_hash("g_fogMaxOpacityReadOnly")
		},
		{
			"g_fogStartDistReadOnly",
			"",
			generate_hash("g_fogStartDistReadOnly")
		},
		{
			"g_friendlyfireDist",
			"Maximum range for disabling fire at a friendly",
			generate_hash("g_friendlyfireDist")
		},
		{
			"g_friendlyNameDist",
			"Maximum range for seeing a friendly's name",
			generate_hash("g_friendlyNameDist")
		},
		{
			"g_gametype",
			"The current game mode",
			generate_hash("g_gametype")
		},
		{
			"g_giveAll",
			"Give all weapons",
			generate_hash("g_giveAll")
		},
		{
			"g_hardcore",
			"Hardcore?",
			generate_hash("g_hardcore")
		},
		{
			"g_heightFogBaseHeightReadOnly",
			"height fog is full density at this world height and below",
			generate_hash("g_heightFogBaseHeightReadOnly")
		},
		{
			"g_heightFogEnabledReadOnly",
			"use height for normal/sun fog, set in the most recent call to \"setexpfog\"",
			generate_hash("g_heightFogEnabledReadOnly")
		},
		{
			"g_heightFogHalfPlaneDistanceReadOnly",
			"at this distance above g_heightFogBaseHeight, height fog density is half, set in the most recent call to \"setexpfog\"",
			generate_hash("g_heightFogHalfPlaneDistanceReadOnly")
		},
		{
			"g_inactivity",
			"Time delay before player is kicked for inactivity",
			generate_hash("g_inactivity")
		},
		{
			"g_keyboarduseholdtime",
			"The time to hold down the 'use' button to activate a 'use' command on a keyboard",
			generate_hash("g_keyboarduseholdtime")
		},
		{
			"g_knockback",
			"Maximum knockback",
			generate_hash("g_knockback")
		},
		{
			"g_lagged_damage_threshold",
			"Threshold (ms) beyond which we will report a damaged lagged client to the tracker leaderboards.",
			generate_hash("g_lagged_damage_threshold")
		},
		{
			"g_listEntity",
			"List the entities",
			generate_hash("g_listEntity")
		},
		{
			"g_mantleBlockTimeBuffer",
			"Time that the client think is delayed after mantling",
			generate_hash("g_mantleBlockTimeBuffer")
		},
		{
			"g_maxDroppedWeapons",
			"Maximum number of dropped weapons",
			generate_hash("g_maxDroppedWeapons")
		},
		{
			"g_minGrenadeDamageSpeed",
			"Minimum speed at which getting hit be a grenade will do damage (not the grenade explosion damage)",
			generate_hash("g_minGrenadeDamageSpeed")
		},
		{
			"g_oldschool",
			"Oldschool?",
			generate_hash("g_oldschool")
		},
		{
			"g_password",
			"Password",
			generate_hash("g_password")
		},
		{
			"g_playerCollisionEjectSpeed",
			"Speed at which to push intersecting players away from each other",
			generate_hash("g_playerCollisionEjectSpeed")
		},
		{
			"g_ScoresColor_Allies",
			"Allies team color on scoreboard",
			generate_hash("g_ScoresColor_Allies")
		},
		{
			"g_ScoresColor_Axis",
			"Axis team color on scoreboard",
			generate_hash("g_ScoresColor_Axis")
		},
		{
			"g_ScoresColor_EnemyTeam",
			"Enemy team color on scoreboard",
			generate_hash("g_ScoresColor_EnemyTeam")
		},
		{
			"g_ScoresColor_Free",
			"Free Team color on scoreboard",
			generate_hash("g_ScoresColor_Free")
		},
		{
			"g_ScoresColor_MyParty",
			"Player team color on scoreboard when in the same party",
			generate_hash("g_ScoresColor_MyParty")
		},
		{
			"g_ScoresColor_MyTeam",
			"Player team color on scoreboard",
			generate_hash("g_ScoresColor_MyTeam")
		},
		{
			"g_ScoresColor_Spectator",
			"Spectator team color on scoreboard",
			generate_hash("g_ScoresColor_Spectator")
		},
		{
			"g_scriptMainMenu",
			"",
			generate_hash("g_scriptMainMenu")
		},
		{
			"g_sunFogBeginFadeAngleReadOnly",
			"Angle from the sun direction to start fade away from the sun fog color that was set in the most recent call to \"setexpfog\"",
			generate_hash("g_sunFogBeginFadeAngleReadOnly")
		},
		{
			"g_sunFogColorIntensityReadOnly",
			"HDR sun fog color intensity that was set in the most recent call to \"setexpfog\"",
			generate_hash("g_sunFogColorIntensityReadOnly")
		},
		{
			"g_sunFogColorReadOnly",
			"Sun fog color that was set in the most recent call to \"setexpfog\"",
			generate_hash("g_sunFogColorReadOnly")
		},
		{
			"g_sunFogDirReadOnly",
			"Sun fog direction that was set in the most recent call to \"setexpfog\"",
			generate_hash("g_sunFogDirReadOnly")
		},
		{
			"g_sunFogEnabledReadOnly",
			"Sun fog was enabled in the most recent call to \"setexpfog\"",
			generate_hash("g_sunFogEnabledReadOnly")
		},
		{
			"g_sunFogEndFadeAngleReadOnly",
			"Angle from the sun direction to end fade away from the sun fog color that was set in the most recent call to \"setexpfog\"",
			generate_hash("g_sunFogEndFadeAngleReadOnly")
		},
		{
			"g_sunFogScaleReadOnly",
			"Distance scale in the sun fog direction that was set in the most recent call to \"setexpfog\"",
			generate_hash("g_sunFogScaleReadOnly")
		},
		{
			"g_TeamIcon_Allies",
			"Icon name for the allied scores banner",
			generate_hash("g_TeamIcon_Allies")
		},
		{
			"g_TeamIcon_Axis",
			"Icon name for the axis scores banner",
			generate_hash("g_TeamIcon_Axis")
		},
		{
			"g_TeamIcon_EnemyAllies",
			"Icon name for the allied scores banner",
			generate_hash("g_TeamIcon_EnemyAllies")
		},
		{
			"g_TeamIcon_EnemyAxis",
			"Icon name for the axis scores banner when you're on axis.",
			generate_hash("g_TeamIcon_EnemyAxis")
		},
		{
			"g_TeamIcon_Free",
			"Icon name for the scores of players with no team",
			generate_hash("g_TeamIcon_Free")
		},
		{
			"g_TeamIcon_MyAllies",
			"Icon name for the allied scores banner",
			generate_hash("g_TeamIcon_MyAllies")
		},
		{
			"g_TeamIcon_MyAxis",
			"Icon name for the axis scores banner when you're on axis.",
			generate_hash("g_TeamIcon_MyAxis")
		},
		{
			"g_TeamIcon_Spectator",
			"Icon name for the scores of players who are spectators",
			generate_hash("g_TeamIcon_Spectator")
		},
		{
			"g_TeamName_Allies",
			"Allied team name",
			generate_hash("g_TeamName_Allies")
		},
		{
			"g_TeamName_Axis",
			"Axis team name",
			generate_hash("g_TeamName_Axis")
		},
		{
			"g_TeamTitleColor_EnemyTeam",
			"Enemy team color for titles",
			generate_hash("g_TeamTitleColor_EnemyTeam")
		},
		{
			"g_TeamTitleColor_MyTeam",
			"Player team color for titles",
			generate_hash("g_TeamTitleColor_MyTeam")
		},
		{
			"g_TeamTitleColor_Spectator",
			"Spectator team color for titles",
			generate_hash("g_TeamTitleColor_Spectator")
		},
		{
			"g_useholdspawndelay",
			"Time in milliseconds that the player is unable to 'use' after spawning",
			generate_hash("g_useholdspawndelay")
		},
		{
			"g_useholdtime",
			"Time to hold the 'use' button to activate use on a gamepad",
			generate_hash("g_useholdtime")
		},
		{
			"g_voiceChatTalkingDuration",
			"Time after the last talk packet was received that the player is considered by the\nserver to still be talking in milliseconds",
			generate_hash("g_voiceChatTalkingDuration")
		},
		{
			"gamedate",
			"The date compiled",
			generate_hash("gamedate")
		},
		{
			"gamedvr_active",
			"Are we allowed to enable GameDVR or not",
			generate_hash("gamedvr_active")
		},
		{
			"gameMode",
			"Current gameMode",
			generate_hash("gameMode")
		},
		{
			"gamename",
			"The name of the game",
			generate_hash("gamename")
		},
		{
			"glass_angular_vel",
			"Sets the range of angular velocities used by new glass pieces",
			generate_hash("glass_angular_vel")
		},
		{
			"glass_beamDamage",
			"The amount of damage beam attacks do to glass",
			generate_hash("glass_beamDamage")
		},
		{
			"glass_break",
			"Toggle whether or not glass breaks when shot",
			generate_hash("glass_break")
		},
		{
			"glass_crack_pattern_scale",
			"The scale applied to the radius used for the crack pattern",
			generate_hash("glass_crack_pattern_scale")
		},
		{
			"glass_damageToDestroy",
			"The amount of damage a piece of glass must take to look damaged",
			generate_hash("glass_damageToDestroy")
		},
		{
			"glass_damageToWeaken",
			"The amount of damage a piece of glass must take to look damaged",
			generate_hash("glass_damageToWeaken")
		},
		{
			"glass_edge_angle",
			"Sets the range of angle deflections used by new glass pieces on a supported edge",
			generate_hash("glass_edge_angle")
		},
		{
			"glass_fall_delay",
			"Sets how long a heavy piece supported by a single edge waits before falling, based on glass_fall_ratio",
			generate_hash("glass_fall_delay")
		},
		{
			"glass_fall_gravity",
			"Gravity for falling pieces of glass",
			generate_hash("glass_fall_gravity")
		},
		{
			"glass_fall_ratio",
			"Ratio of piece area to supporting edge length squared.  Below the min, the piece never falls.",
			generate_hash("glass_fall_ratio")
		},
		{
			"glass_fringe_maxcoverage",
			"The maximum portion of the original piece of glass that is allowed to remain after the glass shatters",
			generate_hash("glass_fringe_maxcoverage")
		},
		{
			"glass_fringe_maxsize",
			"The maximum area for an edge piece of glass when shattering. Pieces larger than this will be broken into smaller ones",
			generate_hash("glass_fringe_maxsize")
		},
		{
			"glass_fx_chance",
			"Chance to play an effect on a small piece of glass when it hits the ground",
			generate_hash("glass_fx_chance")
		},
		{
			"glass_hinge_friction",
			"Friction used by moving glass pieces when joined like a hinge to a frame",
			generate_hash("glass_hinge_friction")
		},
		{
			"glass_linear_vel",
			"Sets the range of linear velocities used by new glass pieces",
			generate_hash("glass_linear_vel")
		},
		{
			"glass_max_pieces_per_frame",
			"Maximum number of pieces to create in one frame. This is a guideline and not a hard limit.",
			generate_hash("glass_max_pieces_per_frame")
		},
		{
			"glass_max_shatter_fx_per_frame",
			"Maximum number of shatter effects to play in one frame This is a guideline and not a hard limit.",
			generate_hash("glass_max_shatter_fx_per_frame")
		},
		{
			"glass_meleeDamage",
			"The amount of damage melee attacks do to glass",
			generate_hash("glass_meleeDamage")
		},
		{
			"glass_physics_chance",
			"The chance for a given shard of glass to use physics",
			generate_hash("glass_physics_chance")
		},
		{
			"glass_physics_maxdist",
			"The maximum distance of a glass piece from the player to do physics",
			generate_hash("glass_physics_maxdist")
		},
		{
			"glass_radiusDamageMultiplier",
			"The amount to scale damage to glass from grenades and other explosions",
			generate_hash("glass_radiusDamageMultiplier")
		},
		{
			"glass_shard_maxsize",
			"The maximum area for a flying piece of glass when shattering. Pieces larger than this will be broken into smaller ones",
			generate_hash("glass_shard_maxsize")
		},
		{
			"glass_shattered_scale",
			"The scale of the shattered glass material",
			generate_hash("glass_shattered_scale")
		},
		{
			"glass_trace_interval",
			"The length of time, in milliseconds, between glass piece traces",
			generate_hash("glass_trace_interval")
		},
		{
			"gpad_button_deadzone",
			"Game pad button deadzone threshhold",
			generate_hash("gpad_button_deadzone")
		},
		{
			"gpad_dpadDebounceTime",
			"",
			generate_hash("gpad_dpadDebounceTime")
		},
		{
			"gpad_menu_scroll_delay_first",
			"Menu scroll key-repeat delay, for the first repeat, in milliseconds",
			generate_hash("gpad_menu_scroll_delay_first")
		},
		{
			"gpad_menu_scroll_delay_rest_accel",
			"Menu scroll key-repeat delay acceleration from start to end, for repeats after the first, in milliseconds per repeat",
			generate_hash("gpad_menu_scroll_delay_rest_accel")
		},
		{
			"gpad_menu_scroll_delay_rest_end",
			"Menu scroll key-repeat delay end, for repeats after the first, in milliseconds",
			generate_hash("gpad_menu_scroll_delay_rest_end")
		},
		{
			"gpad_menu_scroll_delay_rest_start",
			"Menu scroll key-repeat delay start, for repeats after the first, in milliseconds",
			generate_hash("gpad_menu_scroll_delay_rest_start")
		},
		{
			"gpad_stick_deadzone_max",
			"Game pad maximum stick deadzone",
			generate_hash("gpad_stick_deadzone_max")
		},
		{
			"gpad_stick_deadzone_min",
			"Game pad minimum stick deadzone",
			generate_hash("gpad_stick_deadzone_min")
		},
		{
			"gpad_stick_pressed",
			"Game pad stick pressed threshhold",
			generate_hash("gpad_stick_pressed")
		},
		{
			"gpad_stick_pressed_hysteresis",
			"Game pad stick pressed no-change-zone around gpad_stick_pressed to prevent bouncing",
			generate_hash("gpad_stick_pressed_hysteresis")
		},
		{
			"groupDownloadInterval",
			"Minimum interval to wait before getting new group counts",
			generate_hash("groupDownloadInterval")
		},
		{
			"groupUploadInterval",
			"Minimum interval to wait before setting new group counts",
			generate_hash("groupUploadInterval")
		},
		{
			"heli_barrelMaxVelocity",
			"",
			generate_hash("heli_barrelMaxVelocity")
		},
		{
			"heli_barrelRotation",
			"How much to rotate the turret barrel when a helicopter fires",
			generate_hash("heli_barrelRotation")
		},
		{
			"heli_barrelSlowdown",
			"",
			generate_hash("heli_barrelSlowdown")
		},
		{
			"hiDef",
			"True if the game video is running in high-def.",
			generate_hash("hiDef")
		},
		{
			"httpnetfs",
			"Stream fastfiles from the specified http server",
			generate_hash("httpnetfs")
		},
		{
			"hud_bloodOverlayLerpRate",
			"Rate at which blood overlay fades out",
			generate_hash("hud_bloodOverlayLerpRate")
		},
		{
			"hud_deathQuoteFadeTime",
			"The time for the death quote to fade",
			generate_hash("hud_deathQuoteFadeTime")
		},
		{
			"hud_drawHud",
			"",
			generate_hash("hud_drawHud")
		},
		{
			"hud_enable",
			"Enable hud elements",
			generate_hash("hud_enable")
		},
		{
			"hud_fade_ammodisplay",
			"The time for the ammo display to fade in seconds",
			generate_hash("hud_fade_ammodisplay")
		},
		{
			"hud_fade_compass",
			"The time for the compass to fade in seconds",
			generate_hash("hud_fade_compass")
		},
		{
			"hud_fade_healthbar",
			"The time for the health bar to fade in seconds",
			generate_hash("hud_fade_healthbar")
		},
		{
			"hud_fade_offhand",
			"The time for the offhand weapons to fade in seconds",
			generate_hash("hud_fade_offhand")
		},
		{
			"hud_fade_sprint",
			"The time for the sprint meter to fade in seconds",
			generate_hash("hud_fade_sprint")
		},
		{
			"hud_flash_period_offhand",
			"Offhand weapons flash period on changing weapon",
			generate_hash("hud_flash_period_offhand")
		},
		{
			"hud_flash_time_offhand",
			"Offhand weapons flash duration on changing weapon",
			generate_hash("hud_flash_time_offhand")
		},
		{
			"hud_health_pulserate_critical",
			"The pulse rate of the 'critical' pulse effect",
			generate_hash("hud_health_pulserate_critical")
		},
		{
			"hud_health_pulserate_injured",
			"The pulse rate of the 'injured' pulse effect",
			generate_hash("hud_health_pulserate_injured")
		},
		{
			"hud_health_startpulse_critical",
			"The health level at which to start the 'injured' pulse effect",
			generate_hash("hud_health_startpulse_critical")
		},
		{
			"hud_health_startpulse_injured",
			"The health level at which to start the 'injured' pulse effect",
			generate_hash("hud_health_startpulse_injured")
		},
		{
			"hudElemPausedBrightness",
			"Brightness of the hudelems when the game is paused.",
			generate_hash("hudElemPausedBrightness")
		},
		{
			"hudOutlineDuringADS",
			"Turn on the HUD outline (green for friendly, red for enemy) when you are pointing at a player while in ADS.",
			generate_hash("hudOutlineDuringADS")
		},
		{
			"igs_config_dw_filename",
			"Name of the configuration file on DW Publisher storage.",
			generate_hash("igs_config_dw_filename")
		},
		{
			"igs_sosp",
			"Show Original Season Pass",
			generate_hash("igs_sosp")
		},
		{
			"igs_td",
			"Show Trial DLC",
			generate_hash("igs_td")
		},
		{
			"igs_version",
			"Version id for the In-game store. Set version number to 0, to disable update.",
			generate_hash("igs_version")
		},
		{
			"in_mouse",
			"Initialize the mouse drivers",
			generate_hash("in_mouse")
		},
		{
			"inpubliclobby",
			"Currently in a public lobby",
			generate_hash("inpubliclobby")
		},
		{
			"intro",
			"Intro movie should play",
			generate_hash("intro")
		},
		{
			"inventory_addEntitlementsToLocalInventory",
			"bypass the exchange and directly add entitlements to the local cached player inventory.",
			generate_hash("inventory_addEntitlementsToLocalInventory")
		},
		{
			"inventory_enabled",
			"enable/disable the inventory feature",
			generate_hash("inventory_enabled")
		},
		{
			"inventory_enableEntitlementDLCScanning",
			"enable scanning of entitlement DLC.",
			generate_hash("inventory_enableEntitlementDLCScanning")
		},
		{
			"inventory_enableRevoke",
			"Enable revoke on purchases you no longer have rights to.",
			generate_hash("inventory_enableRevoke")
		},
		{
			"inventory_exchangeEnabled",
			"enable/disable the 1st party exchange feature",
			generate_hash("inventory_exchangeEnabled")
		},
		{
			"inventory_exchangeMaxConsumablesPerBoot",
			"The maximum number of the same consumable that can be added per boot.",
			generate_hash("inventory_exchangeMaxConsumablesPerBoot")
		},
		{
			"inventory_exchangeRetryBaseMS",
			"The amount to delay with each subsequent retry as base value to be multiplied by an exponential factor 1000 = (1000, 2000, 4000, 8000 etc.)",
			generate_hash("inventory_exchangeRetryBaseMS")
		},
		{
			"inventory_exchangeRetryByRound",
			"enable/disable retry with exponential delay one round of exchanges at a time (1, 2, 3, 1, 2, 3, 1, 2, 3 etc.), vs exponential delay per exchange (1, 1, 1, 2, 2, 2, 3, 3, 3 etc.)",
			generate_hash("inventory_exchangeRetryByRound")
		},
		{
			"inventory_exchangeRetryMax",
			"The number of times to retry for each exchange.",
			generate_hash("inventory_exchangeRetryMax")
		},
		{
			"inventory_excludeEntitlementDLCScanning",
			"exclude scanning of entitlement DLC (comma separated list of ids to exclude).",
			generate_hash("inventory_excludeEntitlementDLCScanning")
		},
		{
			"inventory_ignoreDWPushNotification_claimAchievement",
			"ignore incoming push notifications from DW to signal item update",
			generate_hash("inventory_ignoreDWPushNotification_claimAchievement")
		},
		{
			"inventory_ignoreDWPushNotification_itemUpdate",
			"ignore incoming push notifications from DW to signal item update",
			generate_hash("inventory_ignoreDWPushNotification_itemUpdate")
		},
		{
			"inventory_taskDefaultTimeout",
			"default timeout for inventory tasks (in seconds)",
			generate_hash("inventory_taskDefaultTimeout")
		},
		{
			"inventory_taskExchangeTimeout",
			"default timeout for inventory exchange tasks (in seconds)",
			generate_hash("inventory_taskExchangeTimeout")
		},
		{
			"inventory_taskGetTimeout",
			"default timeout for inventory GET tasks (in seconds)",
			generate_hash("inventory_taskGetTimeout")
		},
		{
			"inventory_triggerExchangeOnContentMount",
			"trigger an exchange after mounting new content packs",
			generate_hash("inventory_triggerExchangeOnContentMount")
		},
		{
			"inventory_triggerExchangeOnStoreExit",
			"trigger an exchange when exiting the store",
			generate_hash("inventory_triggerExchangeOnStoreExit")
		},
		{
			"iotd_active",
			"Is the IOTD system enabled",
			generate_hash("iotd_active")
		},
		{
			"iotd_retry",
			"Can the IOTD system retry fetching data from Demonware",
			generate_hash("iotd_retry")
		},
		{
			"jump_slowdownEnable",
			"Slow player movement after jumping",
			generate_hash("jump_slowdownEnable")
		},
		{
			"laserDebug",
			"Enables the display of various debug info.",
			generate_hash("laserDebug")
		},
		{
			"laserLightRadius",
			"undefined",
			generate_hash("laserLightRadius")
		},
		{
			"laserRadius",
			"undefined",
			generate_hash("laserRadius")
		},
		{
			"lb_filter",
			"Filter applied to the leaderboard display: ('none','friends','facebook_friends')",
			generate_hash("lb_filter")
		},
		{
			"lb_group",
			"GroupID applied to the leaderboard display",
			generate_hash("lb_group")
		},
		{
			"lb_maxrows",
			"Maximum number of rows to fetch",
			generate_hash("lb_maxrows")
		},
		{
			"lb_minrefresh",
			"Minimum time (in seconds) between leaderboard fetches",
			generate_hash("lb_minrefresh")
		},
		{
			"lb_readDelay",
			"Delay time between reads(in milliseconds) between leaderboard fetches",
			generate_hash("lb_readDelay")
		},
		{
			"lb_throttle_time",
			"Lobby throttling amount",
			generate_hash("lb_throttle_time")
		},
		{
			"lb_times_in_window",
			"Lobby throttling window amount",
			generate_hash("lb_times_in_window")
		},
		{
			"lb_window",
			"Lobby throttling window",
			generate_hash("lb_window")
		},
		{
			"live_qosec_firstupdatems",
			"MS to wait before deciding to early out qos",
			generate_hash("live_qosec_firstupdatems")
		},
		{
			"live_qosec_lastupdatems",
			"MS since last update required to early out qos",
			generate_hash("live_qosec_lastupdatems")
		},
		{
			"live_qosec_minpercent",
			"Minimum percentage of probe results required before early outing qos",
			generate_hash("live_qosec_minpercent")
		},
		{
			"live_qosec_minprobes",
			"Minimum probe results required before early outing qos",
			generate_hash("live_qosec_minprobes")
		},
		{
			"liveanticheatunknowndvar",
			"Live Anti Cheat Unknown Dvar",
			generate_hash("liveanticheatunknowndvar")
		},
		{
			"livestreaming_active",
			"Are we allowed to enable LiveStreaming or not",
			generate_hash("livestreaming_active")
		},
		{
			"loading_sre_fatal",
			"Loading errors prevent level from loading.",
			generate_hash("loading_sre_fatal")
		},
		{
			"lobby_animationSpeed",
			"How long each frame of the animation should draw, in milliseconds",
			generate_hash("lobby_animationSpeed")
		},
		{
			"lobby_animationTilesHigh",
			"How many animation tiles high is the searching_for_player texture",
			generate_hash("lobby_animationTilesHigh")
		},
		{
			"lobby_animationTilesWide",
			"How many animation tiles wide is the searching_for_player texture",
			generate_hash("lobby_animationTilesWide")
		},
		{
			"lobby_numAnimationFrames",
			"How many animation tiles are in the searching_for_player texture",
			generate_hash("lobby_numAnimationFrames")
		},
		{
			"lobby_searchingPartyColor",
			"The color to show that we're searching for that slot when shown in lobbies",
			generate_hash("lobby_searchingPartyColor")
		},
		{
			"loc_language",
			"Language",
			generate_hash("loc_language")
		},
		{
			"loc_translate",
			"Enable translations",
			generate_hash("loc_translate")
		},
		{
			"log_host_migration_chance",
			"The % chance of host migration results telemetry",
			generate_hash("log_host_migration_chance")
		},
		{
			"log_party_state",
			"Log party state updates to Black Box system",
			generate_hash("log_party_state")
		},
		{
			"lowAmmoWarningColor1",
			"Color 1 of 2 to oscilate between",
			generate_hash("lowAmmoWarningColor1")
		},
		{
			"lowAmmoWarningColor2",
			"Color 2 of 2 to oscilate between",
			generate_hash("lowAmmoWarningColor2")
		},
		{
			"lowAmmoWarningNoAmmoColor1",
			"Like lowAmmoWarningColor1, but when no ammo.",
			generate_hash("lowAmmoWarningNoAmmoColor1")
		},
		{
			"lowAmmoWarningNoAmmoColor2",
			"lowAmmoWarningColor2, but when no ammo.",
			generate_hash("lowAmmoWarningNoAmmoColor2")
		},
		{
			"lowAmmoWarningNoReloadColor1",
			"Like lowAmmoWarningColor1, but when no ammo.",
			generate_hash("lowAmmoWarningNoReloadColor1")
		},
		{
			"lowAmmoWarningNoReloadColor2",
			"lowAmmoWarningColor2, but when no ammo.",
			generate_hash("lowAmmoWarningNoReloadColor2")
		},
		{
			"lowAmmoWarningPulseFreq",
			"Frequency of the pulse (oscilation between the 2 colors)",
			generate_hash("lowAmmoWarningPulseFreq")
		},
		{
			"lowAmmoWarningPulseMax",
			"Min of oscilation range: 0 is color1 and 1.0 is color2.  Can be < 0, and the wave will clip at 0.",
			generate_hash("lowAmmoWarningPulseMax")
		},
		{
			"lowAmmoWarningPulseMin",
			"Max of oscilation range: 0 is color1 and 1.0 is color2.  Can be > 1.0, and the wave will clip at 1.0.",
			generate_hash("lowAmmoWarningPulseMin")
		},
		{
			"lsp_enumertion_max_retry_time",
			"Max time that the LSP enumeration can retry",
			generate_hash("lsp_enumertion_max_retry_time")
		},
		{
			"lsp_enumertion_retry_step",
			"Step in m/s for the LSP enumeration retry",
			generate_hash("lsp_enumertion_retry_step")
		},
		{
			"lui_demoMode",
			"Check if the game is in demo mode.",
			generate_hash("lui_demoMode")
		},
		{
			"lui_FFotDSupportEnabled",
			"Enables lui to update itself via the ffotd",
			generate_hash("lui_FFotDSupportEnabled")
		},
		{
			"lui_hud_motion_angle_ease_speed",
			"Hud motion ease percentage of degrees per second",
			generate_hash("lui_hud_motion_angle_ease_speed")
		},
		{
			"lui_hud_motion_bob_scale",
			"Hud motion bob scale",
			generate_hash("lui_hud_motion_bob_scale")
		},
		{
			"lui_hud_motion_enabled",
			"Enable hud motion",
			generate_hash("lui_hud_motion_enabled")
		},
		{
			"lui_hud_motion_perspective",
			"value for hud motion perspective transform in pixels",
			generate_hash("lui_hud_motion_perspective")
		},
		{
			"lui_hud_motion_rotation_max",
			"Hud motion rotation max",
			generate_hash("lui_hud_motion_rotation_max")
		},
		{
			"lui_hud_motion_rotation_scale",
			"Hud motion rotation scale",
			generate_hash("lui_hud_motion_rotation_scale")
		},
		{
			"lui_hud_motion_trans_ease_speed",
			"Hud motion ease percentage of pixels per second",
			generate_hash("lui_hud_motion_trans_ease_speed")
		},
		{
			"lui_hud_motion_translation_max",
			"Hud motion translation max",
			generate_hash("lui_hud_motion_translation_max")
		},
		{
			"lui_hud_motion_translation_scale",
			"Hud motion translation scale",
			generate_hash("lui_hud_motion_translation_scale")
		},
		{
			"lui_loot_duplicateredemption",
			"Whether a user can redeem duplicate loot items in the Armory",
			generate_hash("lui_loot_duplicateredemption")
		},
		{
			"LUI_MemErrorsFatal",
			"Out of memory errors cause drops when true, reinits the UI system if false",
			generate_hash("LUI_MemErrorsFatal")
		},
		{
			"lui_menuFlowEnabled",
			"Enables LUI menu flow",
			generate_hash("lui_menuFlowEnabled")
		},
		{
			"lui_mlg_rules_unlocked",
			"Whether MLG rules are unlocked",
			generate_hash("lui_mlg_rules_unlocked")
		},
		{
			"lui_priv_lobby_team",
			"Team selected in private match lobby",
			generate_hash("lui_priv_lobby_team")
		},
		{
			"lui_splitscreensignin_menu",
			"Enables the LUI splitscreensignin menu",
			generate_hash("lui_splitscreensignin_menu")
		},
		{
			"lui_splitscreenupscaling",
			"Force splitscreen upscaling off/on (-1 off, 1 on) -- requires map change",
			generate_hash("lui_splitscreenupscaling")
		},
		{
			"lui_systemlink_menu",
			"Enables the LUI systemlink menu",
			generate_hash("lui_systemlink_menu")
		},
		{
			"lui_waitingforgavelmessagesconfirmed",
			"",
			generate_hash("lui_waitingforgavelmessagesconfirmed")
		},
		{
			"lui_waitingfornetworktype",
			"value is LuiWaitingForNetworkType enum",
			generate_hash("lui_waitingfornetworktype")
		},
		{
			"lui_waitingforonlinedatafetch_controller",
			"the controller index that is fetching the online stats data",
			generate_hash("lui_waitingforonlinedatafetch_controller")
		},
		{
			"LUI_WorkerCmdGC",
			"Dev-only flag to enable/disable LUI workerCmd GC thread",
			generate_hash("LUI_WorkerCmdGC")
		},
		{
			"lui_xboxlive_menu",
			"Enables the LUI xboxlive menu",
			generate_hash("lui_xboxlive_menu")
		},
		{
			"m_filter",
			"Allow mouse movement smoothing",
			generate_hash("m_filter")
		},
		{
			"m_forward",
			"Forward speed in units per second",
			generate_hash("m_forward")
		},
		{
			"m_pitch",
			"Default pitch",
			generate_hash("m_pitch")
		},
		{
			"m_side",
			"Sideways motion in units per second",
			generate_hash("m_side")
		},
		{
			"m_yaw",
			"Default yaw",
			generate_hash("m_yaw")
		},
		{
			"manifestfs",
			"Use a manifest file to read segmented fastfiles",
			generate_hash("manifestfs")
		},
		{
			"mapname",
			"The current map name",
			generate_hash("mapname")
		},
		{
			"mapPackMPGroupFourFlags",
			"Map pack flags that comprise MP ala carte map pack 1",
			generate_hash("mapPackMPGroupFourFlags")
		},
		{
			"mapPackMPGroupFreeFlags",
			"Map pack flags that comprise the free MP ala carte map pack",
			generate_hash("mapPackMPGroupFreeFlags")
		},
		{
			"mapPackMPGroupOneFlags",
			"Map pack flags that comprise MP ala carte map pack 1",
			generate_hash("mapPackMPGroupOneFlags")
		},
		{
			"mapPackMPGroupThreeFlags",
			"Map pack flags that comprise MP ala carte map pack 1",
			generate_hash("mapPackMPGroupThreeFlags")
		},
		{
			"mapPackMPGroupTwoFlags",
			"Map pack flags that comprise MP ala carte map pack 1",
			generate_hash("mapPackMPGroupTwoFlags")
		},
		{
			"marketing_active",
			"Are we allowed to enable Marketing Comms or not",
			generate_hash("marketing_active")
		},
		{
			"marketing_refresh_time",
			"time in seconds to wait before refreshing marketing messages from demonware",
			generate_hash("marketing_refresh_time")
		},
		{
			"matchdata_active",
			"Are match data uploads enabled",
			generate_hash("matchdata_active")
		},
		{
			"matchdata_maxcompressionbuffer",
			"Max SP match data compression buffer to use (in bytes)",
			generate_hash("matchdata_maxcompressionbuffer")
		},
		{
			"matchmaking_debug",
			"Enable matchmaking debugging information",
			generate_hash("matchmaking_debug")
		},
		{
			"max_ping_threshold_good",
			"max ping value to be considered as good",
			generate_hash("max_ping_threshold_good")
		},
		{
			"max_ping_threshold_medium",
			"max ping value to be considered as medium",
			generate_hash("max_ping_threshold_medium")
		},
		{
			"max_xp_per_match",
			"",
			generate_hash("max_xp_per_match")
		},
		{
			"maxPrestigeOverride",
			"Overrides the maximum prestige level, disabled if 0.",
			generate_hash("maxPrestigeOverride")
		},
		{
			"maxVoicePacketsPerSec",
			"",
			generate_hash("maxVoicePacketsPerSec")
		},
		{
			"maxVoicePacketsPerSecForServer",
			"",
			generate_hash("maxVoicePacketsPerSecForServer")
		},
		{
			"mdsd",
			"enable match data stat delta logging?",
			generate_hash("mdsd")
		},
		{
			"melee_debug",
			"Turn on debug lines for melee traces",
			generate_hash("melee_debug")
		},
		{
			"migration_dvarErrors",
			"Whether to check for illegal script dvar changes.",
			generate_hash("migration_dvarErrors")
		},
		{
			"min_wait_for_players",
			"",
			generate_hash("min_wait_for_players")
		},
		{
			"missileRemoteFOV",
			"Remote missile-cam, FOV to use.",
			generate_hash("missileRemoteFOV")
		},
		{
			"missileRemoteSteerPitchRange",
			"Remote-controlled missile allowed up/down range.  To keep players from steering missiles above the horizon.",
			generate_hash("missileRemoteSteerPitchRange")
		},
		{
			"missileRemoteSteerPitchRate",
			"Remote-controlled missile up/down steering speed.",
			generate_hash("missileRemoteSteerPitchRate")
		},
		{
			"missileRemoteSteerYawRate",
			"Remote-controlled missile left/right steering speed.",
			generate_hash("missileRemoteSteerYawRate")
		},
		{
			"mm_aw_onboarding_rank",
			"If a player is at or above this rank in AW, she is not considered onboarding",
			generate_hash("mm_aw_onboarding_rank")
		},
		{
			"mm_blops2_onboarding_skill",
			"Used to determine onboarding status for Ghosts",
			generate_hash("mm_blops2_onboarding_skill")
		},
		{
			"mm_bucket_option",
			"if using bucketing, describes what pools can join with each other",
			generate_hash("mm_bucket_option")
		},
		{
			"mm_country_code",
			"country code",
			generate_hash("mm_country_code")
		},
		{
			"mm_ghosts_onboarding_skill",
			"Used to determine onboarding status for Ghosts",
			generate_hash("mm_ghosts_onboarding_skill")
		},
		{
			"mm_past_title_stats_source",
			"what type of information do we use from the past titles (rank vs kdr, etc)",
			generate_hash("mm_past_title_stats_source")
		},
		{
			"mm_skill_calculation_type",
			"",
			generate_hash("mm_skill_calculation_type")
		},
		{
			"mm_skill_enforcement",
			"",
			generate_hash("mm_skill_enforcement")
		},
		{
			"mm_skill_lower_bucket",
			"lower mm skill bucket",
			generate_hash("mm_skill_lower_bucket")
		},
		{
			"mm_skill_param_delta",
			"Delta parameter for Johnson SU distribution curve",
			generate_hash("mm_skill_param_delta")
		},
		{
			"mm_skill_param_gamma",
			"Gamma parameter for Johnson SU distribution curve",
			generate_hash("mm_skill_param_gamma")
		},
		{
			"mm_skill_param_lambda",
			"Lambda parameter for Johnson SU distribution curve",
			generate_hash("mm_skill_param_lambda")
		},
		{
			"mm_skill_param_xi",
			"Xi parameter for Johnson SU distribution curve",
			generate_hash("mm_skill_param_xi")
		},
		{
			"mm_skill_strict_enforcement",
			"",
			generate_hash("mm_skill_strict_enforcement")
		},
		{
			"mm_skill_type",
			"mm skill type",
			generate_hash("mm_skill_type")
		},
		{
			"mm_skill_upper_bucket",
			"upper mm skill bucket",
			generate_hash("mm_skill_upper_bucket")
		},
		{
			"mm_sph_1",
			"",
			generate_hash("mm_sph_1")
		},
		{
			"mm_sph_10",
			"",
			generate_hash("mm_sph_10")
		},
		{
			"mm_sph_11",
			"",
			generate_hash("mm_sph_11")
		},
		{
			"mm_sph_12",
			"",
			generate_hash("mm_sph_12")
		},
		{
			"mm_sph_13",
			"",
			generate_hash("mm_sph_13")
		},
		{
			"mm_sph_14",
			"",
			generate_hash("mm_sph_14")
		},
		{
			"mm_sph_15",
			"",
			generate_hash("mm_sph_15")
		},
		{
			"mm_sph_16",
			"",
			generate_hash("mm_sph_16")
		},
		{
			"mm_sph_17",
			"",
			generate_hash("mm_sph_17")
		},
		{
			"mm_sph_18",
			"",
			generate_hash("mm_sph_18")
		},
		{
			"mm_sph_2",
			"",
			generate_hash("mm_sph_2")
		},
		{
			"mm_sph_3",
			"",
			generate_hash("mm_sph_3")
		},
		{
			"mm_sph_4",
			"",
			generate_hash("mm_sph_4")
		},
		{
			"mm_sph_5",
			"",
			generate_hash("mm_sph_5")
		},
		{
			"mm_sph_6",
			"",
			generate_hash("mm_sph_6")
		},
		{
			"mm_sph_7",
			"",
			generate_hash("mm_sph_7")
		},
		{
			"mm_sph_8",
			"",
			generate_hash("mm_sph_8")
		},
		{
			"mm_sph_9",
			"",
			generate_hash("mm_sph_9")
		},
		{
			"mm_split_population",
			"",
			generate_hash("mm_split_population")
		},
		{
			"mm_test_type",
			"mm test type",
			generate_hash("mm_test_type")
		},
		{
			"mm_use_onboarding_skill",
			"If set, we will for the player's skill to be the lowest available",
			generate_hash("mm_use_onboarding_skill")
		},
		{
			"monkeytoy",
			"Restrict console access",
			generate_hash("monkeytoy")
		},
		{
			"motd",
			"",
			generate_hash("motd")
		},
		{
			"motd_store_link",
			"Add a link to the in-game store in the MOTD popup",
			generate_hash("motd_store_link")
		},
		{
			"motionTrackerBlurDuration",
			"The motion blur duration for motion tracker dots",
			generate_hash("motionTrackerBlurDuration")
		},
		{
			"motionTrackerCenterX",
			"",
			generate_hash("motionTrackerCenterX")
		},
		{
			"motionTrackerCenterY",
			"",
			generate_hash("motionTrackerCenterY")
		},
		{
			"motionTrackerPingFadeTime",
			"How long an enemy is visible on the motion tracker after being detected",
			generate_hash("motionTrackerPingFadeTime")
		},
		{
			"motionTrackerPingPitchAddPerEnemy",
			"The added percentage of pitch for each additional enemy that is detected (final pitch = base pitch * (1 + enemy count * this))",
			generate_hash("motionTrackerPingPitchAddPerEnemy")
		},
		{
			"motionTrackerPingPitchBase",
			"The pitch of the motion tracker sound for a nearby enemy",
			generate_hash("motionTrackerPingPitchBase")
		},
		{
			"motionTrackerPingPitchNearby",
			"The pitch of the motion tracker sound for a nearby enemy",
			generate_hash("motionTrackerPingPitchNearby")
		},
		{
			"motionTrackerPingSize",
			"The width and height of the motion tracker's enemy indicators as a percentage of motion tracker scale",
			generate_hash("motionTrackerPingSize")
		},
		{
			"msg_field_delta2",
			"enable the delta2 serialization.",
			generate_hash("msg_field_delta2")
		},
		{
			"name",
			"Player name",
			generate_hash("name")
		},
		{
			"net_authPort",
			"UDP port for Steam authentication",
			generate_hash("net_authPort")
		},
		{
			"net_ip",
			"Network IP Address",
			generate_hash("net_ip")
		},
		{
			"net_masterServerPort",
			"UDP port for Steam server browser",
			generate_hash("net_masterServerPort")
		},
		{
			"net_noudp",
			"Disable UDP",
			generate_hash("net_noudp")
		},
		{
			"net_port",
			"Network port",
			generate_hash("net_port")
		},
		{
			"net_socksEnabled",
			"Enable network sockets",
			generate_hash("net_socksEnabled")
		},
		{
			"net_socksPassword",
			"Network socket password",
			generate_hash("net_socksPassword")
		},
		{
			"net_socksPort",
			"Network socket port",
			generate_hash("net_socksPort")
		},
		{
			"net_socksServer",
			"Network socket server",
			generate_hash("net_socksServer")
		},
		{
			"net_socksUsername",
			"Network socket username",
			generate_hash("net_socksUsername")
		},
		{
			"nextmap",
			"Next map to play",
			generate_hash("nextmap")
		},
		{
			"nightVisionDisableEffects",
			"",
			generate_hash("nightVisionDisableEffects")
		},
		{
			"nightVisionFadeInOutTime",
			"How long the fade to/from black lasts when putting on or removing night vision goggles.",
			generate_hash("nightVisionFadeInOutTime")
		},
		{
			"nightVisionPowerOnTime",
			"How long the black-to-nightvision fade lasts when turning on the goggles.",
			generate_hash("nightVisionPowerOnTime")
		},
		{
			"num_available_map_packs",
			"Number of map packs available for this platform",
			generate_hash("num_available_map_packs")
		},
		{
			"objectiveFontSize",
			"Onscreen Objective Pointer - Fontsize of the icon's text.",
			generate_hash("objectiveFontSize")
		},
		{
			"objectiveTextOffsetY",
			"Onscreen Objective Pointer - Offset of the icon's text.",
			generate_hash("objectiveTextOffsetY")
		},
		{
			"onlinegame",
			"Current game is an online game with stats, custom classes, unlocks",
			generate_hash("onlinegame")
		},
		{
			"overrideNVGModelWithKnife",
			"When true, nightvision animations will attach the weapDef's knife model instead of the night vision goggles.",
			generate_hash("overrideNVGModelWithKnife")
		},
		{
			"overtimeTimeLimit",
			"",
			generate_hash("overtimeTimeLimit")
		},
		{
			"p2pAuth_allow_steam_p2p",
			"Determines if Steam based P2P is allowed if direct connectivity is not possible.",
			generate_hash("p2pAuth_allow_steam_p2p")
		},
		{
			"p2pAuth_disable",
			"",
			generate_hash("p2pAuth_disable")
		},
		{
			"paintExplosionRadius",
			"The radius of the paint grenade explosion",
			generate_hash("paintExplosionRadius")
		},
		{
			"painVisionLerpOutRate",
			"Rate at which pain vision effect lerps out",
			generate_hash("painVisionLerpOutRate")
		},
		{
			"painVisionTriggerHealth",
			"Health (0 to 1) that will trigger the pain vision effect",
			generate_hash("painVisionTriggerHealth")
		},
		{
			"party_alternateMapVoteStatus",
			"Alternate map vote progress",
			generate_hash("party_alternateMapVoteStatus")
		},
		{
			"party_dlc_only_intended_mappacks",
			"When selecting next map for rotation, should any maps not in the intended search be excluded, even if available?",
			generate_hash("party_dlc_only_intended_mappacks")
		},
		{
			"party_firstSubpartyIndex",
			"Determines sort order and coloring of parties in lobbies.  Randomly set by code.  Dvar provided for debugging.",
			generate_hash("party_firstSubpartyIndex")
		},
		{
			"party_followPartyHostOutOfGames",
			"Whether we should follow our party host out of a game in progress.",
			generate_hash("party_followPartyHostOutOfGames")
		},
		{
			"party_gamesize",
			"Current maximum game size",
			generate_hash("party_gamesize")
		},
		{
			"party_gametype",
			"Current gametype",
			generate_hash("party_gametype")
		},
		{
			"party_inactiveHeartbeatPeriod",
			"How often to send inactive party keep alive packets in milliseconds.",
			generate_hash("party_inactiveHeartbeatPeriod")
		},
		{
			"party_initial_dlc_search_timer",
			"Time until DLC enabled search should show an error dialog suggesting the user consider going to non dlc search",
			generate_hash("party_initial_dlc_search_timer")
		},
		{
			"party_IsLocalClientSelected",
			"True if selected player is a local client.  Only valid when used with party feeders.",
			generate_hash("party_IsLocalClientSelected")
		},
		{
			"party_kickplayerquestion",
			"String to store the question about kicking the selected player",
			generate_hash("party_kickplayerquestion")
		},
		{
			"party_listFocus",
			"True when an item in the party list has focus.",
			generate_hash("party_listFocus")
		},
		{
			"party_lobbyPlayerCount",
			"Number of players currently in the party/lobby",
			generate_hash("party_lobbyPlayerCount")
		},
		{
			"party_mapname",
			"Current map name",
			generate_hash("party_mapname")
		},
		{
			"party_mapvote_entrya_mapname",
			"Primary map vote entry name",
			generate_hash("party_mapvote_entrya_mapname")
		},
		{
			"party_mapvote_entryb_mapname",
			"Alternate map vote entry name",
			generate_hash("party_mapvote_entryb_mapname")
		},
		{
			"party_matchedPlayerCount",
			"Number of matched players before revealing their true names",
			generate_hash("party_matchedPlayerCount")
		},
		{
			"party_maxplayers",
			"Maximum number of players in a party",
			generate_hash("party_maxplayers")
		},
		{
			"party_maxPrivatePartyPlayers",
			"Max number of players allowed in a private party.",
			generate_hash("party_maxPrivatePartyPlayers")
		},
		{
			"party_maxTeamDiff",
			"Maximum difference allowed between teams before starting a match",
			generate_hash("party_maxTeamDiff")
		},
		{
			"party_membersMissingMapPack",
			"Whether any party member is missing one of the enabled map packs. Only valid after running partyUpdateMissingMapPackDvar",
			generate_hash("party_membersMissingMapPack")
		},
		{
			"party_minLobbyTime",
			"Minimum time (in seconds) for a lobby to be open before auto starting a match",
			generate_hash("party_minLobbyTime")
		},
		{
			"party_minplayers",
			"Minimum number of players in a party",
			generate_hash("party_minplayers")
		},
		{
			"party_nextMapVoteStatus",
			"Next map vote progress",
			generate_hash("party_nextMapVoteStatus")
		},
		{
			"party_partyPlayerCount",
			"Number of players currently in the party/lobby",
			generate_hash("party_partyPlayerCount")
		},
		{
			"party_partyPlayerCountNum",
			"Number of players currently in the party/lobby",
			generate_hash("party_partyPlayerCountNum")
		},
		{
			"party_playersCoop",
			"True if the current playlist places all players on the allies team",
			generate_hash("party_playersCoop")
		},
		{
			"party_playervisible",
			"Whether selected player in party is showing true info or not.  Only valid when used with party feeders.",
			generate_hash("party_playervisible")
		},
		{
			"party_randomMapVoteStatus",
			"Random map vote progress",
			generate_hash("party_randomMapVoteStatus")
		},
		{
			"party_resume_dlc_search_timer",
			"Time until DLC enabled search should show an error dialog suggesting the user consider going to non dlc search",
			generate_hash("party_resume_dlc_search_timer")
		},
		{
			"party_search_for_dlc_content",
			"search for DLC enabled games else standard maps only will be used",
			generate_hash("party_search_for_dlc_content")
		},
		{
			"party_selectedIndex",
			"Current selected player index in the feeder.",
			generate_hash("party_selectedIndex")
		},
		{
			"party_selectedIndexChangedTime",
			"Time stamp in milliseconds when the selected index last changed.",
			generate_hash("party_selectedIndexChangedTime")
		},
		{
			"party_statusString",
			"Party Status (localized )",
			generate_hash("party_statusString")
		},
		{
			"party_teambased",
			"True if the current playlist is team based",
			generate_hash("party_teambased")
		},
		{
			"party_teamsVisible",
			"teams are visible in UI",
			generate_hash("party_teamsVisible")
		},
		{
			"party_timer",
			"Time until game begins in seconds, for UI display",
			generate_hash("party_timer")
		},
		{
			"partyChatDisallowed",
			"Whether to disallow ps4 Live Party Chat",
			generate_hash("partyChatDisallowed")
		},
		{
			"partymigrate_broadcast_interval",
			"Time between telling people who the new host is when migrating lobby",
			generate_hash("partymigrate_broadcast_interval")
		},
		{
			"partymigrate_cpuBonusPing",
			"The ping rewarded to a CPU meeting the bonus threshold when scoring hosts.",
			generate_hash("partymigrate_cpuBonusPing")
		},
		{
			"partymigrate_cpuBonusThreshold",
			"The required excess %CPU speed to get a bonus when scoring hosts.",
			generate_hash("partymigrate_cpuBonusThreshold")
		},
		{
			"partymigrate_logResults",
			"Instrumentation - Whether to log the best host calculation results. 0 is disabled, 1 for games, 2 for parties, 3 for both.",
			generate_hash("partymigrate_logResults")
		},
		{
			"partymigrate_makeHostTimeout",
			"Time before giving up on makeHost requests",
			generate_hash("partymigrate_makeHostTimeout")
		},
		{
			"partymigrate_pingtest_active",
			"Whether to do a ping test when lobby migrating",
			generate_hash("partymigrate_pingtest_active")
		},
		{
			"partymigrate_pingtest_filterThreshold",
			"Acceptable ping difference from best ping host for host selection (ms)",
			generate_hash("partymigrate_pingtest_filterThreshold")
		},
		{
			"partymigrate_pingtest_minThreshold",
			"Minimum meaningful ping delta for host selection (ms)",
			generate_hash("partymigrate_pingtest_minThreshold")
		},
		{
			"partymigrate_pingtest_retry",
			"Time between ping tests when migrating lobby",
			generate_hash("partymigrate_pingtest_retry")
		},
		{
			"partymigrate_pingtest_timeout",
			"Time to give up on ping tests when migrating lobby",
			generate_hash("partymigrate_pingtest_timeout")
		},
		{
			"partymigrate_preferSameHost",
			"When possible, prefer keeping the same host on migrations",
			generate_hash("partymigrate_preferSameHost")
		},
		{
			"partymigrate_selectiontime",
			"Time before requiring a host selection when migrating lobby",
			generate_hash("partymigrate_selectiontime")
		},
		{
			"partymigrate_timeout",
			"Time before giving up on lobby migration if we hear nothing",
			generate_hash("partymigrate_timeout")
		},
		{
			"partymigrate_timeoutmax",
			"Time before giving up on lobby migration if we hear nothing",
			generate_hash("partymigrate_timeoutmax")
		},
		{
			"partymigrate_uploadtest_minThreshold",
			"Minimum meaningful upload bandwidth delta for host selection (bps)",
			generate_hash("partymigrate_uploadtest_minThreshold")
		},
		{
			"password",
			"",
			generate_hash("password")
		},
		{
			"perk_armorPiercingDamage",
			"",
			generate_hash("perk_armorPiercingDamage")
		},
		{
			"perk_blastShieldScale",
			"",
			generate_hash("perk_blastShieldScale")
		},
		{
			"perk_blastShieldScale_HC",
			"",
			generate_hash("perk_blastShieldScale_HC")
		},
		{
			"perk_bulletPenetrationMultiplier",
			"Multiplier for extra bullet penetration",
			generate_hash("perk_bulletPenetrationMultiplier")
		},
		{
			"perk_extendedMagsMGAmmo",
			"Number of extra bullets per clip for machine gun weapons with the extended mags perk.",
			generate_hash("perk_extendedMagsMGAmmo")
		},
		{
			"perk_extendedMagsPistolAmmo",
			"Number of extra bullets per clip for pistol weapons with the extended mags perk.",
			generate_hash("perk_extendedMagsPistolAmmo")
		},
		{
			"perk_extendedMagsRifleAmmo",
			"Number of extra bullets per clip for rifle weapons with the extended mags perk.",
			generate_hash("perk_extendedMagsRifleAmmo")
		},
		{
			"perk_extendedMagsSMGAmmo",
			"Number of extra bullets per clip for sub machine gun weapons with the extended mags perk.",
			generate_hash("perk_extendedMagsSMGAmmo")
		},
		{
			"perk_extendedMagsSpreadAmmo",
			"Number of extra bullets per clip for spread weapons with the extended mags perk.",
			generate_hash("perk_extendedMagsSpreadAmmo")
		},
		{
			"perk_extraBreath",
			"Number of extra seconds a player can hold his breath",
			generate_hash("perk_extraBreath")
		},
		{
			"perk_fastClimb",
			"Scales the ladder climb time",
			generate_hash("perk_fastClimb")
		},
		{
			"perk_fastRegenRate",
			"",
			generate_hash("perk_fastRegenRate")
		},
		{
			"perk_fastRegenWaitMS",
			"",
			generate_hash("perk_fastRegenWaitMS")
		},
		{
			"perk_fastSnipeScale",
			"Scales the recovery speed of the view kick when using a sniper.",
			generate_hash("perk_fastSnipeScale")
		},
		{
			"perk_footstepVolumeAlly",
			"",
			generate_hash("perk_footstepVolumeAlly")
		},
		{
			"perk_footstepVolumeEnemy",
			"",
			generate_hash("perk_footstepVolumeEnemy")
		},
		{
			"perk_footstepVolumePlayer",
			"",
			generate_hash("perk_footstepVolumePlayer")
		},
		{
			"perk_footstepVolumeSelectiveHearingMin",
			"",
			generate_hash("perk_footstepVolumeSelectiveHearingMin")
		},
		{
			"perk_improvedExtraBreath",
			"Number of extra seconds a player can hold his breath",
			generate_hash("perk_improvedExtraBreath")
		},
		{
			"perk_lightWeightViewBobScale",
			"Scale for first person view movement while lightweight.",
			generate_hash("perk_lightWeightViewBobScale")
		},
		{
			"perk_numExtraLethal",
			"Number of extra lethal grenades",
			generate_hash("perk_numExtraLethal")
		},
		{
			"perk_numExtraTactical",
			"Number of extra tactical grenades",
			generate_hash("perk_numExtraTactical")
		},
		{
			"perk_parabolicAngle",
			"Eavesdrop perk's effective FOV angle",
			generate_hash("perk_parabolicAngle")
		},
		{
			"perk_parabolicIcon",
			"Eavesdrop icon to use when displaying eavesdropped voice chats",
			generate_hash("perk_parabolicIcon")
		},
		{
			"perk_parabolicRadius",
			"Eavesdrop perk's effective radius",
			generate_hash("perk_parabolicRadius")
		},
		{
			"perk_quickDrawSpeedScale",
			"Scales the 'Hip to ADS' transition speed.",
			generate_hash("perk_quickDrawSpeedScale")
		},
		{
			"perk_quickDrawSpeedScaleSniper",
			"Scales the 'Hip to ADS' transition speed.",
			generate_hash("perk_quickDrawSpeedScaleSniper")
		},
		{
			"perk_scavengerMode",
			"",
			generate_hash("perk_scavengerMode")
		},
		{
			"perk_sprintMultiplier",
			"Multiplier for player_sprinttime",
			generate_hash("perk_sprintMultiplier")
		},
		{
			"perk_sprintRecoveryMultiplierActual",
			"",
			generate_hash("perk_sprintRecoveryMultiplierActual")
		},
		{
			"perk_sprintRecoveryMultiplierVisual",
			"",
			generate_hash("perk_sprintRecoveryMultiplierVisual")
		},
		{
			"perk_weapRateMultiplier",
			"Percentage of weapon firing rate to use",
			generate_hash("perk_weapRateMultiplier")
		},
		{
			"perk_weapReloadMultiplier",
			"Percentage of weapon reload time to use",
			generate_hash("perk_weapReloadMultiplier")
		},
		{
			"perk_weapSpreadMultiplier",
			"Percentage of weapon spread to use",
			generate_hash("perk_weapSpreadMultiplier")
		},
		{
			"phys_autoDisableLinear",
			"A body must have linear velocity less than this to be considered idle.",
			generate_hash("phys_autoDisableLinear")
		},
		{
			"phys_autoDisableTime",
			"The amount of time a body must be idle for it to go to sleep.",
			generate_hash("phys_autoDisableTime")
		},
		{
			"phys_bulletSpinScale",
			"Scale of the effective offset from the center of mass for the bullet impacts.",
			generate_hash("phys_bulletSpinScale")
		},
		{
			"phys_bulletUpBias",
			"Up Bias for the direction of the bullet impact.",
			generate_hash("phys_bulletUpBias")
		},
		{
			"phys_dragAngular",
			"The amount of angular drag, applied globally",
			generate_hash("phys_dragAngular")
		},
		{
			"phys_dragLinear",
			"The amount of linear drag, applied globally",
			generate_hash("phys_dragLinear")
		},
		{
			"phys_gravity",
			"Physics gravity in units/sec^2.",
			generate_hash("phys_gravity")
		},
		{
			"phys_gravity_ragdoll",
			"Physics gravity used by ragdolls in units/sec^2.",
			generate_hash("phys_gravity_ragdoll")
		},
		{
			"phys_gravityChangeWakeupRadius",
			"The radius around the player within which objects get awakened when gravity changes",
			generate_hash("phys_gravityChangeWakeupRadius")
		},
		{
			"phys_jitterMaxMass",
			"Maximum mass to jitter - jitter will fall off up to this mass",
			generate_hash("phys_jitterMaxMass")
		},
		{
			"physVeh_explodeForce",
			"The force applied to physics vehicles due to explosions",
			generate_hash("physVeh_explodeForce")
		},
		{
			"physVeh_explodeSpinScale",
			"The max (random) offset from the center of mass at which splash damage applies its force",
			generate_hash("physVeh_explodeSpinScale")
		},
		{
			"physVeh_jump",
			"Set to 1 to make a vehicle jump.",
			generate_hash("physVeh_jump")
		},
		{
			"physVeh_minContactImpulse",
			"The minimum impulse needed to register a contact notification",
			generate_hash("physVeh_minContactImpulse")
		},
		{
			"physVeh_minImpactMomentum",
			"The minimum collision momentum needed to register an impact",
			generate_hash("physVeh_minImpactMomentum")
		},
		{
			"physVeh_StepsPerFrame",
			"The number of physics timesteps that the server frame will be divided into.",
			generate_hash("physVeh_StepsPerFrame")
		},
		{
			"pickupPrints",
			"Print a message to the game window when picking up ammo, etc.",
			generate_hash("pickupPrints")
		},
		{
			"player_breath_snd_delay",
			"The delay before playing the breathe in sound",
			generate_hash("player_breath_snd_delay")
		},
		{
			"player_breath_snd_lerp",
			"The interpolation rate for the player hold breath sound",
			generate_hash("player_breath_snd_lerp")
		},
		{
			"player_current_floor",
			"",
			generate_hash("player_current_floor")
		},
		{
			"player_MGUseRadius",
			"The radius within which a player can mount a machine gun",
			generate_hash("player_MGUseRadius")
		},
		{
			"player_stunWhiteFlash",
			"If enabled, player's screens will flash white when they are stunned.",
			generate_hash("player_stunWhiteFlash")
		},
		{
			"player_throwbackInnerRadius",
			"The radius to a live grenade player must be within initially to do a throwback",
			generate_hash("player_throwbackInnerRadius")
		},
		{
			"player_throwbackOuterRadius",
			"The radius player is allow to throwback a grenade once the player has been in the inner radius",
			generate_hash("player_throwbackOuterRadius")
		},
		{
			"player_useRadius",
			"The radius within which a player can use things",
			generate_hash("player_useRadius")
		},
		{
			"playercard_cache_download_max_retry_time",
			"Max time that the player cache download can retry",
			generate_hash("playercard_cache_download_max_retry_time")
		},
		{
			"playercard_cache_download_retry_step",
			"Step in m/s for the player cache download retry",
			generate_hash("playercard_cache_download_retry_step")
		},
		{
			"playercard_cache_upload_max_retry_time",
			"Max time that the player cache upload can retry",
			generate_hash("playercard_cache_upload_max_retry_time")
		},
		{
			"playercard_cache_upload_retry_step",
			"Step in m/s for the player cache upload retry",
			generate_hash("playercard_cache_upload_retry_step")
		},
		{
			"playercard_cache_validity_life",
			"The life of a cached gamercard (it can be re-downloaded after this)",
			generate_hash("playercard_cache_validity_life")
		},
		{
			"playerPositionRecordSampleTime",
			"How often to sample player positions and save them into match data.",
			generate_hash("playerPositionRecordSampleTime")
		},
		{
			"playlist",
			"The playlist number",
			generate_hash("playlist")
		},
		{
			"playlistAggrFilename",
			"Aggregated playlist filename",
			generate_hash("playlistAggrFilename")
		},
		{
			"playlistFilename",
			"Playlist filename",
			generate_hash("playlistFilename")
		},
		{
			"playListUpdateCheckMinutes",
			"Minutes to wait between checking for updated playlist.",
			generate_hash("playListUpdateCheckMinutes")
		},
		{
			"prestige_shop_active",
			"Are we allowed to show the Prestige Shop or not",
			generate_hash("prestige_shop_active")
		},
		{
			"prestige30EasterEggEnabled",
			"Enables the easter egg for prestige 30 if 1, disabled if 0.",
			generate_hash("prestige30EasterEggEnabled")
		},
		{
			"privateMatch_joinPassword",
			"",
			generate_hash("privateMatch_joinPassword")
		},
		{
			"privateMatch_serverPassword",
			"",
			generate_hash("privateMatch_serverPassword")
		},
		{
			"profileMenuOption_blacklevel",
			"",
			generate_hash("profileMenuOption_blacklevel")
		},
		{
			"profileMenuOption_offensiveContentMode",
			"Mode of the offensive content warning at startup - 0, skip and turn on; 1, skip and turn off; 2, ask user",
			generate_hash("profileMenuOption_offensiveContentMode")
		},
		{
			"profileMenuOption_safeAreaHorz",
			"",
			generate_hash("profileMenuOption_safeAreaHorz")
		},
		{
			"profileMenuOption_safeAreaVert",
			"",
			generate_hash("profileMenuOption_safeAreaVert")
		},
		{
			"profileMenuOption_volume",
			"",
			generate_hash("profileMenuOption_volume")
		},
		{
			"protocol",
			"Protocol version",
			generate_hash("protocol")
		},
		{
			"pt_AliensReadyUpPrivateInUse",
			"Do we use the co-op Ready Up feature in public lobbies?",
			generate_hash("pt_AliensReadyUpPrivateInUse")
		},
		{
			"pt_AliensReadyUpPublicInUse",
			"Do we use the co-op Ready Up feature in public lobbies?",
			generate_hash("pt_AliensReadyUpPublicInUse")
		},
		{
			"pt_AliensReadyUpPublicStartTimerLength",
			"co-op Ready Up start timer length in seconds",
			generate_hash("pt_AliensReadyUpPublicStartTimerLength")
		},
		{
			"pt_allMembersDoQoS",
			"Whether to send search results to all party/lobby members to get QoS data",
			generate_hash("pt_allMembersDoQoS")
		},
		{
			"pt_backoutOnClientPresence",
			"Whether the host should backout the party on client presence. 0=fully disabled, 1=out of game only, 2=in-game also",
			generate_hash("pt_backoutOnClientPresence")
		},
		{
			"pt_connectAttempts",
			"Connect timeout when joining another game/party, per attempt",
			generate_hash("pt_connectAttempts")
		},
		{
			"pt_connectTimeout",
			"Connect timeout when joining another game/party, per attempt",
			generate_hash("pt_connectTimeout")
		},
		{
			"pt_gameStartTimerLength",
			"Time in seconds before a game start once enough party members are ready",
			generate_hash("pt_gameStartTimerLength")
		},
		{
			"pt_logHostSelectionChance",
			"Sets the probability that we log our host selection results",
			generate_hash("pt_logHostSelectionChance")
		},
		{
			"pt_memberTimeout",
			"Client timeout time in the general case",
			generate_hash("pt_memberTimeout")
		},
		{
			"pt_migrateBeforeAdvertise",
			"Whether lobbies made by private parties should migrate host before publishing",
			generate_hash("pt_migrateBeforeAdvertise")
		},
		{
			"pt_migrationBandwidthBonusPing",
			"The ping rewarded to the bonus bandwidth threshold when scoring hosts.",
			generate_hash("pt_migrationBandwidthBonusPing")
		},
		{
			"pt_migrationBandwidthBonusThreshold",
			"The required excess % upload bandwidth to get a bonus when scoring hosts.",
			generate_hash("pt_migrationBandwidthBonusThreshold")
		},
		{
			"pt_migrationCPUWeight",
			"How important CPU speed is when selecting a new host",
			generate_hash("pt_migrationCPUWeight")
		},
		{
			"pt_migrationNotInstalledWeight",
			"How important not being done installing is when selecting a new host",
			generate_hash("pt_migrationNotInstalledWeight")
		},
		{
			"pt_migrationPingBad",
			"",
			generate_hash("pt_migrationPingBad")
		},
		{
			"pt_migrationPingWeight",
			"",
			generate_hash("pt_migrationPingWeight")
		},
		{
			"pt_migrationQuitsBad",
			"",
			generate_hash("pt_migrationQuitsBad")
		},
		{
			"pt_migrationQuitsWeight",
			"",
			generate_hash("pt_migrationQuitsWeight")
		},
		{
			"pt_migrationRAMWeight",
			"How important it is to have the minimum amount of RAM when selecting a new host",
			generate_hash("pt_migrationRAMWeight")
		},
		{
			"pt_migrationThreshold",
			"Minimum amount which another client must be better than the current host to trigger a migration",
			generate_hash("pt_migrationThreshold")
		},
		{
			"pt_migrationUploadBad",
			"",
			generate_hash("pt_migrationUploadBad")
		},
		{
			"pt_migrationUploadWeight",
			"",
			generate_hash("pt_migrationUploadWeight")
		},
		{
			"pt_migrationWifiPenalty",
			"How important Wifi is when selecting a new host",
			generate_hash("pt_migrationWifiPenalty")
		},
		{
			"pt_pregameStartTimerLength",
			"Time in seconds before showing and starting the game start timer",
			generate_hash("pt_pregameStartTimerLength")
		},
		{
			"pt_rejoin",
			"Enable defensive rejoin command",
			generate_hash("pt_rejoin")
		},
		{
			"pt_reservedAnonymousSlotTime",
			"Time in milliseconds that ANONYMOUS slots will be reserved.",
			generate_hash("pt_reservedAnonymousSlotTime")
		},
		{
			"pt_reservedCommittedSlotTime",
			"Time in milliseconds that COMMITTED slots will be reserved",
			generate_hash("pt_reservedCommittedSlotTime")
		},
		{
			"pt_reservedJoiningSlotTime",
			"Time in milliseconds that JOINING slots will be reserved",
			generate_hash("pt_reservedJoiningSlotTime")
		},
		{
			"pt_searchConnectAttempts",
			"Connect timeout when joining another game/party, per attempt",
			generate_hash("pt_searchConnectAttempts")
		},
		{
			"pt_searchPauseTime",
			"Minimum amount of time to pause between searches",
			generate_hash("pt_searchPauseTime")
		},
		{
			"pt_searchRandomDelay",
			"Time period over which the search timers will get randomly delayed.",
			generate_hash("pt_searchRandomDelay")
		},
		{
			"pt_searchResultsLifetime",
			"Time at which we consider the search results stale",
			generate_hash("pt_searchResultsLifetime")
		},
		{
			"pt_searchResultsMin",
			"Minimum amount of time that has to pass before we'll search again for matches",
			generate_hash("pt_searchResultsMin")
		},
		{
			"pt_stillConnectingWaitTime",
			"Amount of time to wait for someone to finish connecting before searching for lobbies to merge with",
			generate_hash("pt_stillConnectingWaitTime")
		},
		{
			"pt_useMigrationWeights",
			"Killswitch to turn on or off the host selection by weights",
			generate_hash("pt_useMigrationWeights")
		},
		{
			"publisherFileFetchTimeout",
			"default timeout for publisher files FETCH tasks (in seconds)",
			generate_hash("publisherFileFetchTimeout")
		},
		{
			"r_adaptiveSubdiv",
			"Enables screen space Catmull Clark adaptive tessellation.  If disabled, models tessellate to their designed subdivision level.",
			generate_hash("r_adaptiveSubdiv")
		},
		{
			"r_adaptiveSubdivBaseFactor",
			"Screen space Catmull Clark adaptive tessellation factor for the base model.  Smaller values mean more tessellation.",
			generate_hash("r_adaptiveSubdivBaseFactor")
		},
		{
			"r_adaptiveSubdivPatchFactor",
			"Screen space Catmull Clark adaptive tessellation factor for the base model.  Smaller values mean more tessellation.",
			generate_hash("r_adaptiveSubdivPatchFactor")
		},
		{
			"r_allCells",
			"Draw all cells.  Most useful for seeing if portals or cells are hiding things they should not..",
			generate_hash("r_allCells")
		},
		{
			"r_amdGPU",
			"At least on AMD GPU used for rendering.",
			generate_hash("r_amdGPU")
		},
		{
			"r_aoBlurSharpness",
			"Controls the tolerance for depth discontinuities during the bilateral blur step.  Larger values reduce the depth tolerance and effectively sharpen more edges.",
			generate_hash("r_aoBlurSharpness")
		},
		{
			"r_aoBlurStep",
			"Step scale applied to sample offsets during the bilateral blur.  A value of 1 results in a normal gaussian blur.  Increasing it to 2 or 3 makes the filter larger but causes fine dithering patterns.",
			generate_hash("r_aoBlurStep")
		},
		{
			"r_aoDiminish",
			"Decrease the effect of occlusion on brighter colors",
			generate_hash("r_aoDiminish")
		},
		{
			"r_aoPower",
			"Power curve applied to AO factor",
			generate_hash("r_aoPower")
		},
		{
			"r_aoStrength",
			"Strength of Ambient Occlusion effect",
			generate_hash("r_aoStrength")
		},
		{
			"r_aoUseTweaks",
			"Use r_ao* dvars instead of the current light set values for AO common params",
			generate_hash("r_aoUseTweaks")
		},
		{
			"r_artUseTweaks",
			"Tells the game that art tweaks is enabled and script is in control (as opposed to ColorEd).",
			generate_hash("r_artUseTweaks")
		},
		{
			"r_aspectRatio",
			"Screen aspect ratio.  Most widescreen monitors are 16:10 instead of 16:9.",
			generate_hash("r_aspectRatio")
		},
		{
			"r_asyncCompute",
			"Enables scheduling GPU compute shader work prior to the graphics frame, improving overlap.",
			generate_hash("r_asyncCompute")
		},
		{
			"r_atlasAnimFPS",
			"Speed to animate atlased 2d materials",
			generate_hash("r_atlasAnimFPS")
		},
		{
			"r_autopriority",
			"Automatically set the priority of the windows process when the game is minimized",
			generate_hash("r_autopriority")
		},
		{
			"r_balanceLightmapOpaqueLists",
			"Split lightmap opaque into multiple draw lists.",
			generate_hash("r_balanceLightmapOpaqueLists")
		},
		{
			"r_blacklevel",
			"Black level (negative brightens output)",
			generate_hash("r_blacklevel")
		},
		{
			"r_blur",
			"Dev tweak to blur the screen",
			generate_hash("r_blur")
		},
		{
			"r_blurdstGaussianBlurLevel",
			"MIP level to start gaussian blur at",
			generate_hash("r_blurdstGaussianBlurLevel")
		},
		{
			"r_blurdstGaussianBlurRadius",
			"Amount to gaussian blur blur distortion render target",
			generate_hash("r_blurdstGaussianBlurRadius")
		},
		{
			"r_brightness",
			"Brightness adjustment",
			generate_hash("r_brightness")
		},
		{
			"r_cacheModelLighting",
			"Speed up model lighting by caching previous results",
			generate_hash("r_cacheModelLighting")
		},
		{
			"r_cacheSModelLighting",
			"Speed up static model lighting by caching previous results",
			generate_hash("r_cacheSModelLighting")
		},
		{
			"r_charLightAmbient",
			"",
			generate_hash("r_charLightAmbient")
		},
		{
			"r_clampLodScale",
			"Clamps the amount that the engine can adjust the LOD distance. 0 the engine can fully adjust. 1 the engine cannot adjust it at all. 0.5 the maximum the engine can adjust the LOD distance is 50% or the default.",
			generate_hash("r_clampLodScale")
		},
		{
			"r_clear",
			"Controls how the color buffer is cleared",
			generate_hash("r_clear")
		},
		{
			"r_clearColor",
			"Color to clear the screen to when clearing the frame buffer",
			generate_hash("r_clearColor")
		},
		{
			"r_clearColor2",
			"Color to clear every second frame to (for use during development)",
			generate_hash("r_clearColor2")
		},
		{
			"r_clutCompositeVisionSet",
			"Composite clut with vision set.",
			generate_hash("r_clutCompositeVisionSet")
		},
		{
			"r_cmdbuf_worker",
			"Process command buffer in a separate thread",
			generate_hash("r_cmdbuf_worker")
		},
		{
			"r_colorGradingEnable",
			"Enable color grading.",
			generate_hash("r_colorGradingEnable")
		},
		{
			"r_colorMap",
			"Replace all color maps with pure black or pure white",
			generate_hash("r_colorMap")
		},
		{
			"r_colorScaleUseTweaks",
			"Override color scale LightSet settings with tweak dvar values. (MP)",
			generate_hash("r_colorScaleUseTweaks")
		},
		{
			"r_combinePostOpaqueFx",
			"",
			generate_hash("r_combinePostOpaqueFx")
		},
		{
			"r_contrast",
			"Contrast adjustment",
			generate_hash("r_contrast")
		},
		{
			"r_darkBlur",
			"Apply blur (decrease of visual acuity) when dark",
			generate_hash("r_darkBlur")
		},
		{
			"r_darkBlurPower",
			"Power curve of blurring (decrease of visual acuity) when dark",
			generate_hash("r_darkBlurPower")
		},
		{
			"r_darkBlurRadius",
			"Radius of blurring (decrease of visual acuity) when dark",
			generate_hash("r_darkBlurRadius")
		},
		{
			"r_darkColor",
			"Reduce color sensitivity when dark",
			generate_hash("r_darkColor")
		},
		{
			"r_darkColorPower",
			"Power curve of color sensitivity when dark",
			generate_hash("r_darkColorPower")
		},
		{
			"r_debugLineWidth",
			"Width of server side debug lines",
			generate_hash("r_debugLineWidth")
		},
		{
			"r_defaultPatchCount",
			"Patches per thread group for all other surfaces.",
			generate_hash("r_defaultPatchCount")
		},
		{
			"r_depthPrepass",
			"Enable depth prepass for various geometries",
			generate_hash("r_depthPrepass")
		},
		{
			"r_depthSortEnable",
			"Enable sorting of transparent surfaces.",
			generate_hash("r_depthSortEnable")
		},
		{
			"r_depthSortRange",
			"Range to consider depth sort,",
			generate_hash("r_depthSortRange")
		},
		{
			"r_desaturation",
			"Desaturation adjustment",
			generate_hash("r_desaturation")
		},
		{
			"r_detailMap",
			"Replace all detail maps with an image that effectively disables them",
			generate_hash("r_detailMap")
		},
		{
			"r_diffuseColorScale",
			"Globally scale the diffuse color of all point lights",
			generate_hash("r_diffuseColorScale")
		},
		{
			"r_displacementMap",
			"Replace all displacement maps with an image that effectively disables them",
			generate_hash("r_displacementMap")
		},
		{
			"r_displacementPatchCount",
			"Patches per thread group for displacement surfaces.",
			generate_hash("r_displacementPatchCount")
		},
		{
			"r_distortion",
			"Enable distortion",
			generate_hash("r_distortion")
		},
		{
			"r_distortion_script_force_off",
			"Force distortion off in script",
			generate_hash("r_distortion_script_force_off")
		},
		{
			"r_dlightLimit",
			"Maximum number of dynamic lights drawn simultaneously",
			generate_hash("r_dlightLimit")
		},
		{
			"r_dof_bias",
			"Depth of field bias as a power function (like gamma); less than 1 is sharper",
			generate_hash("r_dof_bias")
		},
		{
			"r_dof_enable",
			"Enable the depth of field effect",
			generate_hash("r_dof_enable")
		},
		{
			"r_dof_farBlur",
			"",
			generate_hash("r_dof_farBlur")
		},
		{
			"r_dof_farEnd",
			"Depth of field far end distance, in inches",
			generate_hash("r_dof_farEnd")
		},
		{
			"r_dof_farStart",
			"Depth of field far start distance, in inches",
			generate_hash("r_dof_farStart")
		},
		{
			"r_dof_nearBlur",
			"",
			generate_hash("r_dof_nearBlur")
		},
		{
			"r_dof_nearEnd",
			"Depth of field near end distance, in inches",
			generate_hash("r_dof_nearEnd")
		},
		{
			"r_dof_nearStart",
			"Depth of field near start distance, in inches",
			generate_hash("r_dof_nearStart")
		},
		{
			"r_dof_physical_adsFocusSpeed",
			"ADS focus speed (focus dist. far to near, focus dist. near to far, aperture opening, aperture closing)",
			generate_hash("r_dof_physical_adsFocusSpeed")
		},
		{
			"r_dof_physical_adsMaxFstop",
			"ADS maximum f-stop (optimal aperture and focus distance are automatically calculated for this mode)",
			generate_hash("r_dof_physical_adsMaxFstop")
		},
		{
			"r_dof_physical_adsMinFstop",
			"ADS minimum f-stop (optimal aperture and focus distance are automatically calculated for this mode)",
			generate_hash("r_dof_physical_adsMinFstop")
		},
		{
			"r_dof_physical_bokehEnable",
			"Enable the bokeh depth of field effect",
			generate_hash("r_dof_physical_bokehEnable")
		},
		{
			"r_dof_physical_bokehPreset",
			"Changes dof sampling quality",
			generate_hash("r_dof_physical_bokehPreset")
		},
		{
			"r_dof_physical_bokehRotation",
			"Bokeh shape rotation in degrees (hexagonal and octogonal only)",
			generate_hash("r_dof_physical_bokehRotation")
		},
		{
			"r_dof_physical_bokehShape",
			"Changes the bokeh shape",
			generate_hash("r_dof_physical_bokehShape")
		},
		{
			"r_dof_physical_bokehSharpness",
			"Bokeh shape sharpness, trades sharpness for stability (circular only)",
			generate_hash("r_dof_physical_bokehSharpness")
		},
		{
			"r_dof_physical_enable",
			"enable physical camera controls (using aperture priority)",
			generate_hash("r_dof_physical_enable")
		},
		{
			"r_dof_physical_filmDiagonal",
			"Diagonal size of the film/sensor (mm). The bigger the sensor size, the bigger the circle of confusion (which means stronger blurring at all distances). Defaults to full-frame 35mm",
			generate_hash("r_dof_physical_filmDiagonal")
		},
		{
			"r_dof_physical_focusDistance",
			"Distance to the plane in focus for the scene",
			generate_hash("r_dof_physical_focusDistance")
		},
		{
			"r_dof_physical_fstop",
			"Aperture of the camera for the scene. Lower f-stop yields a shallower depth of field. Typical values range from f/1 to f/22. Rare extremes are f/0.75 and f/32",
			generate_hash("r_dof_physical_fstop")
		},
		{
			"r_dof_physical_hipEnable",
			"Enable hyperfocal mode",
			generate_hash("r_dof_physical_hipEnable")
		},
		{
			"r_dof_physical_hipFocusSpeed",
			"Hyperfocal mode focus speed (focus dist. far to near, focus dist. near to far, aperture opening, aperture closing)",
			generate_hash("r_dof_physical_hipFocusSpeed")
		},
		{
			"r_dof_physical_hipFstop",
			"Aperture of the camera for the scene in the hyperfocal mode",
			generate_hash("r_dof_physical_hipFstop")
		},
		{
			"r_dof_physical_hipSharpCocDiameter",
			"Defines what circle of confusion can be considered sharp (mm). Defaults to 0.03mm, generally accepted value for 35mm",
			generate_hash("r_dof_physical_hipSharpCocDiameter")
		},
		{
			"r_dof_physical_maxCocDiameter",
			"Maximum circle of confusion diameter (virtual units, might be clamped for bokeh dof)",
			generate_hash("r_dof_physical_maxCocDiameter")
		},
		{
			"r_dof_physical_minFocusDistance",
			"Minimum focus distance (inches)",
			generate_hash("r_dof_physical_minFocusDistance")
		},
		{
			"r_dof_physical_viewModelFocusDistance",
			"Distance to the plane in focus for the scene",
			generate_hash("r_dof_physical_viewModelFocusDistance")
		},
		{
			"r_dof_physical_viewModelFstop",
			"Aperture of the camera for the view model. Lower f-stop yields a shallower depth of field. Typical values range from f/1 to f/22. Rare extremes are f/0.75 and f/32",
			generate_hash("r_dof_physical_viewModelFstop")
		},
		{
			"r_dof_tweak",
			"Use dvars to set the depth of field effect; overrides r_dof_enable",
			generate_hash("r_dof_tweak")
		},
		{
			"r_dof_viewModelEnd",
			"Depth of field viewmodel end distance, in inches",
			generate_hash("r_dof_viewModelEnd")
		},
		{
			"r_dof_viewModelStart",
			"Depth of field viewmodel start distance, in inches",
			generate_hash("r_dof_viewModelStart")
		},
		{
			"r_drawSun",
			"Enable sun effects",
			generate_hash("r_drawSun")
		},
		{
			"r_drawWater",
			"Enable water animation",
			generate_hash("r_drawWater")
		},
		{
			"r_dynamicOPL",
			"Enable drawing vfx lights as overlapping primary light for saving gpu performance.",
			generate_hash("r_dynamicOPL")
		},
		{
			"r_dynamicSpotLightShadows",
			"Enable shadows for dynamic/VFX spot lights, you should set this dvar then spawn the new light.",
			generate_hash("r_dynamicSpotLightShadows")
		},
		{
			"r_elevatedPriority",
			"Utilize priority elevation for process.",
			generate_hash("r_elevatedPriority")
		},
		{
			"r_emblemBrightnessScale",
			"Modifier that scales the brightness of the emblem on model materials",
			generate_hash("r_emblemBrightnessScale")
		},
		{
			"r_emissiveMap",
			"Replace all emissive maps with pure black or pure white",
			generate_hash("r_emissiveMap")
		},
		{
			"r_enableNoTessBuckets",
			"Enables placing triangles that don't need tessellation into additional draw calls using non-tessellated shaders.",
			generate_hash("r_enableNoTessBuckets")
		},
		{
			"r_envBrdfLutMap",
			"Replace environment BRDF lookup table with pure black (no secondary specular) or pure white (maximum secondary specular)",
			generate_hash("r_envBrdfLutMap")
		},
		{
			"r_envMapExponent",
			"Reflection exponent.",
			generate_hash("r_envMapExponent")
		},
		{
			"r_envMapMaxIntensity",
			"Max reflection intensity based on glancing angle.",
			generate_hash("r_envMapMaxIntensity")
		},
		{
			"r_envMapMinIntensity",
			"",
			generate_hash("r_envMapMinIntensity")
		},
		{
			"r_envMapOverride",
			"",
			generate_hash("r_envMapOverride")
		},
		{
			"r_envMapSunIntensity",
			"Max sun specular intensity intensity with env map materials.",
			generate_hash("r_envMapSunIntensity")
		},
		{
			"r_eyePupil",
			" Change eye's pupil Size.",
			generate_hash("r_eyePupil")
		},
		{
			"r_eyeRedness",
			" Change eye's redness.",
			generate_hash("r_eyeRedness")
		},
		{
			"r_eyeWetness",
			" Change eye's wetness.",
			generate_hash("r_eyeWetness")
		},
		{
			"r_fastModelPrimaryLightCheck",
			"Reduce R_GetNonSunPrimaryLightForSphere/R_GetNonSunPrimaryLightForBox function calls",
			generate_hash("r_fastModelPrimaryLightCheck")
		},
		{
			"r_fastModelPrimaryLightLink",
			"Speed up R_LinkSphereEntityToPrimaryLights and R_LinkBoxEntityToPrimaryLights.",
			generate_hash("r_fastModelPrimaryLightLink")
		},
		{
			"r_filmAltShader",
			"Use alternate shader (with middle tint and dark desat) for film color.",
			generate_hash("r_filmAltShader")
		},
		{
			"r_filmTweakBrightness",
			"Tweak dev var; film color brightness",
			generate_hash("r_filmTweakBrightness")
		},
		{
			"r_filmTweakContrast",
			"Tweak dev var; film color contrast",
			generate_hash("r_filmTweakContrast")
		},
		{
			"r_filmTweakDarkTint",
			"",
			generate_hash("r_filmTweakDarkTint")
		},
		{
			"r_filmTweakDesaturation",
			"Tweak dev var; Desaturation applied after all 3D drawing to light areas",
			generate_hash("r_filmTweakDesaturation")
		},
		{
			"r_filmTweakDesaturationDark",
			"Tweak dev var; Additional desaturation applied after all 3D drawing to dark areas",
			generate_hash("r_filmTweakDesaturationDark")
		},
		{
			"r_filmTweakEnable",
			"Tweak dev var; enable film color effects",
			generate_hash("r_filmTweakEnable")
		},
		{
			"r_filmTweakInvert",
			"Tweak dev var; enable inverted video",
			generate_hash("r_filmTweakInvert")
		},
		{
			"r_filmTweakLightTint",
			"",
			generate_hash("r_filmTweakLightTint")
		},
		{
			"r_filmTweakMediumTint",
			"",
			generate_hash("r_filmTweakMediumTint")
		},
		{
			"r_filmUseTweaks",
			"Overide film effects with tweak dvar values.",
			generate_hash("r_filmUseTweaks")
		},
		{
			"r_flushAfterExecute",
			"Whether to Flush after ExecuteCommandList.",
			generate_hash("r_flushAfterExecute")
		},
		{
			"r_fog",
			"Set to 0 to disable fog",
			generate_hash("r_fog")
		},
		{
			"r_fog_depthhack_scale",
			"Fog scale for depth hack surfaces",
			generate_hash("r_fog_depthhack_scale")
		},
		{
			"r_fog_ev_adjust",
			"Fog color ev adjustment (+2 means fog color is 2 stops brighter)",
			generate_hash("r_fog_ev_adjust")
		},
		{
			"r_font_cache_debug_display",
			"Display the current fontcache texture on the HUD for debug purposes",
			generate_hash("r_font_cache_debug_display")
		},
		{
			"r_forceLod",
			"Force all level of detail to this level",
			generate_hash("r_forceLod")
		},
		{
			"r_fullbright",
			"Toggles rendering without lighting",
			generate_hash("r_fullbright")
		},
		{
			"r_fxaaSubpixel",
			"FXAA sub-pixel amount, lower values have more aliasing and less blur",
			generate_hash("r_fxaaSubpixel")
		},
		{
			"r_FXAverageColorFunc",
			"How to compute FX system average color? 0 = use IWrad equation, 1 = legacy equation, 2 = spherical harmonics 1 coefficient.",
			generate_hash("r_FXAverageColorFunc")
		},
		{
			"r_globalGenericMaterialScale",
			"Hack global generic material constants",
			generate_hash("r_globalGenericMaterialScale")
		},
		{
			"r_glow_allowed",
			"Allow glow.",
			generate_hash("r_glow_allowed")
		},
		{
			"r_glow_allowed_script_forced",
			"Force 'allow glow' to be treated as true, by script.",
			generate_hash("r_glow_allowed_script_forced")
		},
		{
			"r_gunSightColorEntityScale",
			"Scale the gun sight color when over an entity.",
			generate_hash("r_gunSightColorEntityScale")
		},
		{
			"r_gunSightColorNoneScale",
			"Scale the gun sight color when not over an entity.",
			generate_hash("r_gunSightColorNoneScale")
		},
		{
			"r_hbaoBias",
			"HBAO bias",
			generate_hash("r_hbaoBias")
		},
		{
			"r_hbaoBlurEnable",
			"HBAO blur enabled",
			generate_hash("r_hbaoBlurEnable")
		},
		{
			"r_hbaoBlurSharpness",
			"HBAO blur sharpness",
			generate_hash("r_hbaoBlurSharpness")
		},
		{
			"r_hbaoCoarseAO",
			"HBAO coarse AO",
			generate_hash("r_hbaoCoarseAO")
		},
		{
			"r_hbaoDebug",
			"Debug render HBAO occlusion",
			generate_hash("r_hbaoDebug")
		},
		{
			"r_hbaoDetailAO",
			"HBAO detail AO",
			generate_hash("r_hbaoDetailAO")
		},
		{
			"r_hbaoPowerExponent",
			"HBAO power exponent",
			generate_hash("r_hbaoPowerExponent")
		},
		{
			"r_hbaoRadius",
			"HBAO radius",
			generate_hash("r_hbaoRadius")
		},
		{
			"r_hbaoSceneScale",
			"HBAO scene scale",
			generate_hash("r_hbaoSceneScale")
		},
		{
			"r_hbaoStrengthBlend",
			"Blend factor between the artist-tuned proportional strength r_hbaoStrengthScale*artStrength, and the fixed strength r_hbaoStrengthFixed.  A value of 0.0 is fully the proportional value, and a value of 1.0 is fully the fixed value.",
			generate_hash("r_hbaoStrengthBlend")
		},
		{
			"r_hbaoStrengthFixed",
			"Fixed HBAO strength.  Only used if r_hbaoStrengthBlend > 0.0.",
			generate_hash("r_hbaoStrengthFixed")
		},
		{
			"r_hbaoStrengthScale",
			"Scale factor to convert SSAO strength to HBAO strength.  Only used if r_hbaoStrengthBlend < 1.0.",
			generate_hash("r_hbaoStrengthScale")
		},
		{
			"r_hbaoUseScriptScale",
			"Enable/disable script-controlled strength scale while HBAO is active.",
			generate_hash("r_hbaoUseScriptScale")
		},
		{
			"r_hemiAoBlurTolerance",
			"Hemi SSAO Blur Tolerance (log10)",
			generate_hash("r_hemiAoBlurTolerance")
		},
		{
			"r_hemiAoCombineResolutionsBeforeBlur",
			"The higher quality modes blend wide and narrow sampling patterns. The wide pattern is due to deinterleaving and requires blurring. The narrow pattern is not on a deinterleaved buffer, but it only samples every other pixel. The blur on it is optional. If you combine the two before blurring, the narrow will get blurred as well. This creates a softer effect but can remove any visible noise from having 50% sample coverage.",
			generate_hash("r_hemiAoCombineResolutionsBeforeBlur")
		},
		{
			"r_hemiAoCombineResolutionsWithMul",
			"When combining the wide and narrow patterns, a mul() operation can be used or a min() operation. Multiplication exaggerates the result creating even darker creases. This is an artistic choice. I think it looks less natural, but often art teams prefer more exaggerated contrast. For me, it's more about having the right AO falloff so that it's a smooth gradient rather than falling off precipitously and forming overly dark recesses.",
			generate_hash("r_hemiAoCombineResolutionsWithMul")
		},
		{
			"r_hemiAoDepthSquash",
			"Hemi SSAO depth squash. Value is rcp.",
			generate_hash("r_hemiAoDepthSquash")
		},
		{
			"r_hemiAoEnable",
			"Enable Hemi SSAO",
			generate_hash("r_hemiAoEnable")
		},
		{
			"r_hemiAoHierarchyDepth",
			"Hemi SSAO recursion depth (filter width)",
			generate_hash("r_hemiAoHierarchyDepth")
		},
		{
			"r_hemiAoMaxDepthDownsample",
			"Use max depth value when downsampling, instead of pseudo-randomly picking a depth sample? Leaving this at the default false may produce more stable results.",
			generate_hash("r_hemiAoMaxDepthDownsample")
		},
		{
			"r_hemiAoNoiseFilterTolerance",
			"This is necessary to filter out pixel shimmer due to bilateral upsampling with too much lost resolution. High frequency detail can sometimes not be reconstructed, and the noise filter fills in the missing pixels with the result of the higher resolution SSAO. Value is log10.",
			generate_hash("r_hemiAoNoiseFilterTolerance")
		},
		{
			"r_hemiAoPower",
			"Power curve applied to Hemi SSAO factor, not applied in game yet",
			generate_hash("r_hemiAoPower")
		},
		{
			"r_hemiAoQualityLevel",
			"Hemi SSAO quality setting",
			generate_hash("r_hemiAoQualityLevel")
		},
		{
			"r_hemiAoRejectionFalloff",
			"Controls how aggressive to fade off samples that occlude spheres but by so much as to be unreliable. This is what gives objects a dark halo around them when placed in front of a wall. If you want to fade off the halo, boost your rejection falloff. The tradeoff is that it reduces overall AO. Value is rcp.",
			generate_hash("r_hemiAoRejectionFalloff")
		},
		{
			"r_hemiAoStrength",
			"Strength of Hemi Screen Space Ambient Occlusion effect",
			generate_hash("r_hemiAoStrength")
		},
		{
			"r_hemiAoUpsampleTolerance",
			"Hemi SSAO Upsample Tolerance (log10)",
			generate_hash("r_hemiAoUpsampleTolerance")
		},
		{
			"r_heroLighting",
			"Enable hero-only lighting",
			generate_hash("r_heroLighting")
		},
		{
			"r_highLodDist",
			"Distance for high level of detail",
			generate_hash("r_highLodDist")
		},
		{
			"r_hudFx",
			"Draw HUD Effects",
			generate_hash("r_hudFx")
		},
		{
			"r_hudOutlineEnable",
			"Enables wireframe outlines to be drawn around DObjs (as a post process).",
			generate_hash("r_hudOutlineEnable")
		},
		{
			"r_hudOutlinePostMode",
			"hud outline apply mode",
			generate_hash("r_hudOutlinePostMode")
		},
		{
			"r_hudOutlineWidth",
			"Set the width of the Hud Outline",
			generate_hash("r_hudOutlineWidth")
		},
		{
			"r_ignore",
			"",
			generate_hash("r_ignore")
		},
		{
			"r_ignoref",
			"",
			generate_hash("r_ignoref")
		},
		{
			"r_imageQuality",
			"Image quality",
			generate_hash("r_imageQuality")
		},
		{
			"r_inGameVideo",
			"Allow in game cinematics",
			generate_hash("r_inGameVideo")
		},
		{
			"r_lateAllocParamCacheAllowed",
			"Enable late allocation of parameter cache for VS stage.",
			generate_hash("r_lateAllocParamCacheAllowed")
		},
		{
			"r_lateAllocParamCacheDefault",
			"Late allocation of parameter cache value for sub-div materials.",
			generate_hash("r_lateAllocParamCacheDefault")
		},
		{
			"r_lateAllocParamCacheDisplacement",
			"Late allocation of parameter cache value for sub-div materials.",
			generate_hash("r_lateAllocParamCacheDisplacement")
		},
		{
			"r_lateAllocParamCacheSubdiv",
			"Late allocation of parameter cache value for sub-div materials.",
			generate_hash("r_lateAllocParamCacheSubdiv")
		},
		{
			"r_lightCacheLessFrequentMaxDistance",
			"Adjust the distance fx models (and models tagged as less-frequently-lit by script) move before immediately being relit",
			generate_hash("r_lightCacheLessFrequentMaxDistance")
		},
		{
			"r_lightCacheLessFrequentPeriod",
			"Adjust how frequently fx models (and models tagged as less-frequently-lit by script) get relit on average (1 is every frame, 8 is every 8th frame)",
			generate_hash("r_lightCacheLessFrequentPeriod")
		},
		{
			"r_lightGridAvgApplyPrimaryLight",
			"apply primary light color onto r_showLightGridAvgProbes boxes",
			generate_hash("r_lightGridAvgApplyPrimaryLight")
		},
		{
			"r_lightGridAvgFollowCamera",
			"allow the r_showLightGridAvgProbes boxes following current camera position",
			generate_hash("r_lightGridAvgFollowCamera")
		},
		{
			"r_lightGridAvgProbeCount",
			"how many light grid avg color probes will show up)",
			generate_hash("r_lightGridAvgProbeCount")
		},
		{
			"r_lightGridAvgTraceGround",
			" lock boxes to ground ",
			generate_hash("r_lightGridAvgTraceGround")
		},
		{
			"r_lightGridContrast",
			"Adjust the contrast of light color from the light grid",
			generate_hash("r_lightGridContrast")
		},
		{
			"r_lightGridDefaultFXLightingLookup",
			"Default FX lighting lookup location\n",
			generate_hash("r_lightGridDefaultFXLightingLookup")
		},
		{
			"r_lightGridDefaultModelLightingLookup",
			"Default model lighting lookup location",
			generate_hash("r_lightGridDefaultModelLightingLookup")
		},
		{
			"r_lightGridEnableTweaks",
			"Enable tweaks of the light color from the light grid",
			generate_hash("r_lightGridEnableTweaks")
		},
		{
			"r_lightGridIntensity",
			"Adjust the intensity of light color from the light grid",
			generate_hash("r_lightGridIntensity")
		},
		{
			"r_lightGridSHBands",
			"Spherical harmonics bands being used for evaluating current-gen light grids colors. 0 = default, 1 = 1 band, 2 = 2 bands, 3 = 3 bands.\n",
			generate_hash("r_lightGridSHBands")
		},
		{
			"r_lightGridUseTweakedValues",
			"Use tweaked values instead of default",
			generate_hash("r_lightGridUseTweakedValues")
		},
		{
			"r_lightMap",
			"Replace all lightmaps with pure black or pure white",
			generate_hash("r_lightMap")
		},
		{
			"r_litSurfaceHDRScalar",
			"Vision set based scalar applied to lit surfaces",
			generate_hash("r_litSurfaceHDRScalar")
		},
		{
			"r_loadForRenderer",
			"Set to false to disable dx allocations (for dedicated server mode)",
			generate_hash("r_loadForRenderer")
		},
		{
			"r_lockPvs",
			"Lock the viewpoint used for determining what is visible to the current position and direction",
			generate_hash("r_lockPvs")
		},
		{
			"r_lod4Dist",
			"Distance for lowest level of detail 4",
			generate_hash("r_lod4Dist")
		},
		{
			"r_lod5Dist",
			"Distance for lowest level of detail 5",
			generate_hash("r_lod5Dist")
		},
		{
			"r_lodBiasRigid",
			"",
			generate_hash("r_lodBiasRigid")
		},
		{
			"r_lodBiasSkinned",
			"",
			generate_hash("r_lodBiasSkinned")
		},
		{
			"r_lodScaleRigid",
			"",
			generate_hash("r_lodScaleRigid")
		},
		{
			"r_lodScaleSkinned",
			"",
			generate_hash("r_lodScaleSkinned")
		},
		{
			"r_lowestLodDist",
			"Distance for lowest level of detail",
			generate_hash("r_lowestLodDist")
		},
		{
			"r_lowLodDist",
			"Distance for low level of detail",
			generate_hash("r_lowLodDist")
		},
		{
			"r_mbEnable",
			"Set of objects which will be enabled for motion blur",
			generate_hash("r_mbEnable")
		},
		{
			"r_mbFastEnable",
			"Toggle on/off fast high quality motion blur",
			generate_hash("r_mbFastEnable")
		},
		{
			"r_mbFastPreset",
			"Changes motion blur quality",
			generate_hash("r_mbFastPreset")
		},
		{
			"r_mdao",
			"Enable the medium distance ambient occlusion feature",
			generate_hash("r_mdao")
		},
		{
			"r_mdaoAsyncOccluderGen",
			"The occluder generation step is performed via async compute",
			generate_hash("r_mdaoAsyncOccluderGen")
		},
		{
			"r_mdaoBoneInfluenceRadiusScale",
			"Scale for the bone influence radius for mdao",
			generate_hash("r_mdaoBoneInfluenceRadiusScale")
		},
		{
			"r_mdaoCapsuleStrength",
			"MDAO strength for capsule occluders",
			generate_hash("r_mdaoCapsuleStrength")
		},
		{
			"r_mdaoMinBoneBoundsToOcclude",
			"Minimum volume of the bone collider to create occluders for",
			generate_hash("r_mdaoMinBoneBoundsToOcclude")
		},
		{
			"r_mdaoOccluderCullDistance",
			"Culling distance for mdao occluders",
			generate_hash("r_mdaoOccluderCullDistance")
		},
		{
			"r_mdaoOccluderFadeOutStartDistance",
			"Fade out distance for mdao occluders",
			generate_hash("r_mdaoOccluderFadeOutStartDistance")
		},
		{
			"r_mdaoUseTweaks",
			"Use r_mdao* dvars instead of the current light set values for MDAO",
			generate_hash("r_mdaoUseTweaks")
		},
		{
			"r_mdaoVolumeStrength",
			"MDAO strength for volume occluders",
			generate_hash("r_mdaoVolumeStrength")
		},
		{
			"r_mediumLodDist",
			"Distance for medium level of detail",
			generate_hash("r_mediumLodDist")
		},
		{
			"r_mode",
			"Display mode",
			generate_hash("r_mode")
		},
		{
			"r_modelLightingMap",
			"Replace all model lighting maps (light grid) with pure black",
			generate_hash("r_modelLightingMap")
		},
		{
			"r_monitor",
			"Index of the monitor to use in a multi monitor system; 0 picks automatically.",
			generate_hash("r_monitor")
		},
		{
			"r_mpRimColor",
			"Change character's rim color for multiplayer",
			generate_hash("r_mpRimColor")
		},
		{
			"r_mpRimDiffuseTint",
			"Change character's rim diffuse tint for multiplayer.",
			generate_hash("r_mpRimDiffuseTint")
		},
		{
			"r_mpRimStrength",
			"Change character's rim color for multiplayer",
			generate_hash("r_mpRimStrength")
		},
		{
			"r_multiGPU",
			"Enable multi GPU compat mode.",
			generate_hash("r_multiGPU")
		},
		{
			"r_normalMap",
			"Replace all normal maps with a flat normal map",
			generate_hash("r_normalMap")
		},
		{
			"r_nvidiaGPU",
			"Enable NV API.",
			generate_hash("r_nvidiaGPU")
		},
		{
			"r_offchipTessellationAllowed",
			"Enable off-chip tessellation support.",
			generate_hash("r_offchipTessellationAllowed")
		},
		{
			"r_offchipTessellationTfThreshold",
			"Tessellation factor threshold for off-chip.",
			generate_hash("r_offchipTessellationTfThreshold")
		},
		{
			"r_offchipTessellationWaveThreshold",
			"Domain shader wave threshold for off-chip.",
			generate_hash("r_offchipTessellationWaveThreshold")
		},
		{
			"r_omitUnusedRenderTargets",
			"Omit unused render targets to save memory.  Changing this requires a vid_restart.",
			generate_hash("r_omitUnusedRenderTargets")
		},
		{
			"r_outdoor",
			"Prevents snow from going indoors",
			generate_hash("r_outdoor")
		},
		{
			"r_outdoorFeather",
			"Outdoor z-feathering value",
			generate_hash("r_outdoorFeather")
		},
		{
			"r_particleHdr",
			"Enable Hdr Particle Features",
			generate_hash("r_particleHdr")
		},
		{
			"r_patchCountAllowed",
			"Enable run-time setting of patch count per draw call.",
			generate_hash("r_patchCountAllowed")
		},
		{
			"r_picmip",
			"Picmip level of color maps.  If r_picmip_manual is 0, this is read-only.",
			generate_hash("r_picmip")
		},
		{
			"r_picmip_bump",
			"Picmip level of normal maps.  If r_picmip_manual is 0, this is read-only.",
			generate_hash("r_picmip_bump")
		},
		{
			"r_picmip_spec",
			"Picmip level of specular maps.  If r_picmip_manual is 0, this is read-only.",
			generate_hash("r_picmip_spec")
		},
		{
			"r_picmip_water",
			"Picmip level of water maps.",
			generate_hash("r_picmip_water")
		},
		{
			"r_polygonOffsetBias",
			"Offset bias for decal polygons; bigger values z-fight less but poke through walls more",
			generate_hash("r_polygonOffsetBias")
		},
		{
			"r_polygonOffsetClamp",
			"Offset clamp for decal polygons; bigger values z-fight less but poke through walls more",
			generate_hash("r_polygonOffsetClamp")
		},
		{
			"r_polygonOffsetScale",
			"Offset scale for decal polygons; bigger values z-fight less but poke through walls more",
			generate_hash("r_polygonOffsetScale")
		},
		{
			"r_portalBevels",
			"Helps cull geometry by angles of portals that are acute when projected onto the screen, value is the cosine of the angle",
			generate_hash("r_portalBevels")
		},
		{
			"r_portalBevelsOnly",
			"Use screen-space bounding box of portals rather than the actual shape of the portal projected onto the screen",
			generate_hash("r_portalBevelsOnly")
		},
		{
			"r_portalMinClipArea",
			"Don't clip child portals by a parent portal smaller than this fraction of the screen area.",
			generate_hash("r_portalMinClipArea")
		},
		{
			"r_portalMinRecurseDepth",
			"Ignore r_portalMinClipArea for portals with fewer than this many parent portals.",
			generate_hash("r_portalMinRecurseDepth")
		},
		{
			"r_portalWalkLimit",
			"Stop portal recursion after this many iterations.  Useful for debugging portal errors.",
			generate_hash("r_portalWalkLimit")
		},
		{
			"r_postAA",
			"Post process antialiasing mode",
			generate_hash("r_postAA")
		},
		{
			"r_postfx_enable",
			"Enable post-processing effects such as color correction, bloom, depth-of-field, etc.",
			generate_hash("r_postfx_enable")
		},
		{
			"r_preloadShaders",
			"Force D3D to draw dummy geometry with all shaders during level load; may fix long pauses at level start.",
			generate_hash("r_preloadShaders")
		},
		{
			"r_primaryLightTweakDiffuseStrength",
			"Tweak the diffuse intensity for primary lights",
			generate_hash("r_primaryLightTweakDiffuseStrength")
		},
		{
			"r_primaryLightTweakSpecularStrength",
			"Tweak the specular intensity for primary lights",
			generate_hash("r_primaryLightTweakSpecularStrength")
		},
		{
			"r_primaryLightUseTweaks",
			"",
			generate_hash("r_primaryLightUseTweaks")
		},
		{
			"r_reactiveMotionActorRadius",
			"Radial distance from the ai characters that influences reactive motion models (inches)",
			generate_hash("r_reactiveMotionActorRadius")
		},
		{
			"r_reactiveMotionActorVelocityMax",
			"AI velocity considered the maximum when determining the length of motion tails (inches/sec)",
			generate_hash("r_reactiveMotionActorVelocityMax")
		},
		{
			"r_reactiveMotionActorZOffset",
			"Distance from the actor origin along Z direction where the actor's reactive motion effector sphere is centered at.",
			generate_hash("r_reactiveMotionActorZOffset")
		},
		{
			"r_reactiveMotionEffectorStrengthScale",
			"Additional scale for the effector influence, as a factor of the model part distance from the effector center and model part extents",
			generate_hash("r_reactiveMotionEffectorStrengthScale")
		},
		{
			"r_reactiveMotionHelicopterLimit",
			"Maximum number of helicopter entities that actively influence reactive motion.  Can increase CPU cost of the scene.",
			generate_hash("r_reactiveMotionHelicopterLimit")
		},
		{
			"r_reactiveMotionHelicopterRadius",
			"Radial distance from the helicopter that influences reactive motion models (inches)",
			generate_hash("r_reactiveMotionHelicopterRadius")
		},
		{
			"r_reactiveMotionHelicopterStrength",
			"Scales the influence of helicopter wind tunnel motion",
			generate_hash("r_reactiveMotionHelicopterStrength")
		},
		{
			"r_reactiveMotionPlayerHeightAdjust",
			"Amount to adjust the vertical distance of the effector from the player position (inches)",
			generate_hash("r_reactiveMotionPlayerHeightAdjust")
		},
		{
			"r_reactiveMotionPlayerRadius",
			"Radial distance from the player that influences reactive motion models (inches)",
			generate_hash("r_reactiveMotionPlayerRadius")
		},
		{
			"r_reactiveMotionPlayerZOffset",
			"Distance from the player origin along Z direction where the player's reactive motion effector sphere is centered at.",
			generate_hash("r_reactiveMotionPlayerZOffset")
		},
		{
			"r_reactiveMotionVelocityTailScale",
			"Additional scale for the velocity-based motion tails, as a factor of the effector radius",
			generate_hash("r_reactiveMotionVelocityTailScale")
		},
		{
			"r_reactiveMotionWindAmplitudeScale",
			"Scales amplitude of wind wave motion",
			generate_hash("r_reactiveMotionWindAmplitudeScale")
		},
		{
			"r_reactiveMotionWindAreaScale",
			"Scales distribution of wind motion",
			generate_hash("r_reactiveMotionWindAreaScale")
		},
		{
			"r_reactiveMotionWindDir",
			"Controls the global wind direction",
			generate_hash("r_reactiveMotionWindDir")
		},
		{
			"r_reactiveMotionWindFrequencyScale",
			"Scales frequency of wind wave motion",
			generate_hash("r_reactiveMotionWindFrequencyScale")
		},
		{
			"r_reactiveMotionWindStrength",
			"Scale of the global wind direction (inches/sec)",
			generate_hash("r_reactiveMotionWindStrength")
		},
		{
			"r_reflectionProbeMap",
			"Replace all reflection probes with pure black",
			generate_hash("r_reflectionProbeMap")
		},
		{
			"r_reflectionProbeNmlLuminance",
			"Enable/disable shader code for computing luminance during reflection probe denormalization. This is just an experiment.\n",
			generate_hash("r_reflectionProbeNmlLuminance")
		},
		{
			"r_rimLight0Color",
			"",
			generate_hash("r_rimLight0Color")
		},
		{
			"r_rimLight0Heading",
			"Rim Light 0 heading in degrees",
			generate_hash("r_rimLight0Heading")
		},
		{
			"r_rimLight0Pitch",
			"Rim Light 0 pitch in degrees -90 is noon.",
			generate_hash("r_rimLight0Pitch")
		},
		{
			"r_rimLightBias",
			"How much to bias the n.l calculation",
			generate_hash("r_rimLightBias")
		},
		{
			"r_rimLightDiffuseIntensity",
			"Strength of the diffuse component of the rim light.",
			generate_hash("r_rimLightDiffuseIntensity")
		},
		{
			"r_rimLightFalloffMaxDistance",
			"Distance at which the rim light hits intensity of 100%.",
			generate_hash("r_rimLightFalloffMaxDistance")
		},
		{
			"r_rimLightFalloffMinDistance",
			"Distance at which the rim light hits intensity of 100%.",
			generate_hash("r_rimLightFalloffMinDistance")
		},
		{
			"r_rimLightFalloffMinIntensity",
			"Intensity of the effect at and before minDistance.",
			generate_hash("r_rimLightFalloffMinIntensity")
		},
		{
			"r_rimLightPower",
			"Power to raise the n.l calculation",
			generate_hash("r_rimLightPower")
		},
		{
			"r_rimLightSpecIntensity",
			"Strength of the spec ( additive) component of the rim light",
			generate_hash("r_rimLightSpecIntensity")
		},
		{
			"r_rimLightUseTweaks",
			"Turn on rim lighting tweaks",
			generate_hash("r_rimLightUseTweaks")
		},
		{
			"r_scaleViewport",
			"Scale 3D viewports by this fraction.  Use this to see if framerate is pixel shader bound.",
			generate_hash("r_scaleViewport")
		},
		{
			"r_sceneMipShowOverlay",
			"Toggles scene mip rendertarget overlay",
			generate_hash("r_sceneMipShowOverlay")
		},
		{
			"r_showLightGrid",
			"Show light grid debugging information (2: detailed, 3: detailed for this box only)",
			generate_hash("r_showLightGrid")
		},
		{
			"r_showLightGridAvgProbes",
			"show an array of boxes which are using the light grid average color at its location",
			generate_hash("r_showLightGridAvgProbes")
		},
		{
			"r_showLightGridDetailInfo",
			"Show more details for light grid debugging.",
			generate_hash("r_showLightGridDetailInfo")
		},
		{
			"r_showLightProbes",
			"Show the light probes at the light grid sample locations in world space centered around the camera.",
			generate_hash("r_showLightProbes")
		},
		{
			"r_showMissingLightGrid",
			"Use rainbow colors for entities that are outside the light grid",
			generate_hash("r_showMissingLightGrid")
		},
		{
			"r_showModelLightingLowWaterMark",
			"",
			generate_hash("r_showModelLightingLowWaterMark")
		},
		{
			"r_showPortals",
			"Show portals for debugging",
			generate_hash("r_showPortals")
		},
		{
			"r_showPortalsOverview",
			"Render 2d XY portal overlay scaled to fit to this distance.  Useful for debugging portal errors.",
			generate_hash("r_showPortalsOverview")
		},
		{
			"r_showReflectionProbeSelection",
			"Show reflection probe selection",
			generate_hash("r_showReflectionProbeSelection")
		},
		{
			"r_singleCell",
			"Only draw things in the same cell as the camera.  Most useful for seeing how big the current cell is.",
			generate_hash("r_singleCell")
		},
		{
			"r_skipPvs",
			"Skipt the determination of what is in the potentially visible set (disables most drawing)",
			generate_hash("r_skipPvs")
		},
		{
			"r_sky_fog_intensity",
			"Amount of sky fog fading",
			generate_hash("r_sky_fog_intensity")
		},
		{
			"r_sky_fog_max_angle",
			"End of angular sky fog fading",
			generate_hash("r_sky_fog_max_angle")
		},
		{
			"r_sky_fog_min_angle",
			"Start of angular sky fog fading",
			generate_hash("r_sky_fog_min_angle")
		},
		{
			"r_skyFogUseTweaks",
			"Enable dvar control of sky fog",
			generate_hash("r_skyFogUseTweaks")
		},
		{
			"r_smaaThreshold",
			"SMAA edge detection threshold",
			generate_hash("r_smaaThreshold")
		},
		{
			"r_smodelInstancedRenderer",
			"Render static models with instanced renderer",
			generate_hash("r_smodelInstancedRenderer")
		},
		{
			"r_smodelInstancedThreshold",
			"Minimum number of static model instances before instanced rendering is used",
			generate_hash("r_smodelInstancedThreshold")
		},
		{
			"r_smp_backend",
			"Process renderer back end in a separate thread",
			generate_hash("r_smp_backend")
		},
		{
			"r_smp_worker",
			"Process renderer front end in a separate thread",
			generate_hash("r_smp_worker")
		},
		{
			"r_smp_worker_thread0",
			"",
			generate_hash("r_smp_worker_thread0")
		},
		{
			"r_smp_worker_thread1",
			"",
			generate_hash("r_smp_worker_thread1")
		},
		{
			"r_smp_worker_thread2",
			"",
			generate_hash("r_smp_worker_thread2")
		},
		{
			"r_smp_worker_thread3",
			"undefined",
			generate_hash("r_smp_worker_thread3")
		},
		{
			"r_smp_worker_thread4",
			"undefined",
			generate_hash("r_smp_worker_thread4")
		},
		{
			"r_smp_worker_thread5",
			"undefined",
			generate_hash("r_smp_worker_thread5")
		},
		{
			"r_smp_worker_thread6",
			"undefined",
			generate_hash("r_smp_worker_thread6")
		},
		{
			"r_smp_worker_thread7",
			"undefined",
			generate_hash("r_smp_worker_thread7")
		},
		{
			"r_specOccMap",
			"Replace all specular occlusion maps with pure black (fully occluded) or pure white (not occluded)",
			generate_hash("r_specOccMap")
		},
		{
			"r_specularColorScale",
			"Set greater than 1 to brighten specular highlights",
			generate_hash("r_specularColorScale")
		},
		{
			"r_specularMap",
			"Replace all specular maps with pure black (off) or pure white (super shiny)",
			generate_hash("r_specularMap")
		},
		{
			"r_spotLightEntityShadows",
			"Enable entity shadows for spot lights.",
			generate_hash("r_spotLightEntityShadows")
		},
		{
			"r_spotLightShadows",
			"Enable shadows for spot lights.",
			generate_hash("r_spotLightShadows")
		},
		{
			"r_ssao",
			"Screen Space Ambient Occlusion mode",
			generate_hash("r_ssao")
		},
		{
			"r_ssaoDebug",
			"Render calculated or blurred Screen Space Ambient Occlusion values",
			generate_hash("r_ssaoDebug")
		},
		{
			"r_ssaoDebugMip",
			"Selects which mip level to render when r_ssaoDebug is enabled. If 0 and r_ssaoDownsample is enabled, will render mip 1.",
			generate_hash("r_ssaoDebugMip")
		},
		{
			"r_ssaoDepthScale",
			"Scale applied to depth values used for occlusion tests.",
			generate_hash("r_ssaoDepthScale")
		},
		{
			"r_ssaoDepthScaleViewModel",
			"Scale applied to depth values used for occlusion tests.",
			generate_hash("r_ssaoDepthScaleViewModel")
		},
		{
			"r_ssaoDownsample",
			"Screen Space Ambient Occlusion calculation occurs at half linear resolution",
			generate_hash("r_ssaoDownsample")
		},
		{
			"r_ssaoFadeDepth",
			"Depth at which the SSAO begins to fade out.  It fades at even increments of this distance (e.g. it's at 1 for depth r_ssaoFadeDepth, 1/2 for depth 2*r_ssaoFadeDepth, etc.)",
			generate_hash("r_ssaoFadeDepth")
		},
		{
			"r_ssaoGapFalloff",
			"Falloff used to blend between creases (that should darken) and silhouettes (that should not darken).  Lower values falloff more quickly.",
			generate_hash("r_ssaoGapFalloff")
		},
		{
			"r_ssaoGradientFalloff",
			"Falloff used to fade out the effect for steep depth gradients (i.e. surfaces nearly parallel to the camera direction).  This fixes sampling artifacts that appear for surfaces nearly parallel to the camera direction (commonly occuring for flat ground planes).",
			generate_hash("r_ssaoGradientFalloff")
		},
		{
			"r_ssaoMaxStrengthDepth",
			"Depth at which SSAO strength is at its maximum",
			generate_hash("r_ssaoMaxStrengthDepth")
		},
		{
			"r_ssaoMethod",
			"Screen Space Ambient Occlusion method (original or IW6, both are volumetric obscurance)",
			generate_hash("r_ssaoMethod")
		},
		{
			"r_ssaoMinPixelWidth",
			"Minimum pixel width of the effect.  When the effect is smaller than this, it is culled entirely.",
			generate_hash("r_ssaoMinPixelWidth")
		},
		{
			"r_ssaoMinStrengthDepth",
			"Depth at which SSAO strength is zero, effectively disabled",
			generate_hash("r_ssaoMinStrengthDepth")
		},
		{
			"r_ssaoMultiRes",
			"Screen Space Ambient Occlusion calculation occurs at half linear resolution",
			generate_hash("r_ssaoMultiRes")
		},
		{
			"r_ssaoPower",
			"Power curve applied to SSAO factor",
			generate_hash("r_ssaoPower")
		},
		{
			"r_ssaoRejectDepth",
			"Depth at which the SSAO is disabled.  Smaller values result in more rejected pixels which is faster, but limits the distance at which the effect is visible.",
			generate_hash("r_ssaoRejectDepth")
		},
		{
			"r_ssaoSampleCount",
			"Selects the number of samples used for SSAO",
			generate_hash("r_ssaoSampleCount")
		},
		{
			"r_ssaoScriptScale",
			"Allows script to lerp to disable or enable the SSAO.  This applies a scalar value to the SSAO strength.  When set to 0, this effectively disables SSAO.",
			generate_hash("r_ssaoScriptScale")
		},
		{
			"r_ssaoStrength",
			"Strength of Screen Space Ambient Occlusion effect",
			generate_hash("r_ssaoStrength")
		},
		{
			"r_ssaoUseTweaks",
			"Use r_ssao* dvars instead of the current light set values for SSAO",
			generate_hash("r_ssaoUseTweaks")
		},
		{
			"r_ssaoWidth",
			"The width of the SSAO effect, in pixels at 720p.  Larger values increase area but lower effective quality.",
			generate_hash("r_ssaoWidth")
		},
		{
			"r_sse_skinning",
			"Use Streaming SIMD Extensions for skinning",
			generate_hash("r_sse_skinning")
		},
		{
			"r_ssrBlendScale",
			"Add extra scale to ssr weight versus reflection probe weight, >1 value will make ssr more obvious.",
			generate_hash("r_ssrBlendScale")
		},
		{
			"r_ssrFadeInDuration",
			"Duration of the screen-space reflection fade-in, which occurs whenever the reflection source buffer is invalidated due to view changes (in particular, dual-view scope transitions).",
			generate_hash("r_ssrFadeInDuration")
		},
		{
			"r_ssrPositionCorrection",
			"Screen space reflection position correction blend factor",
			generate_hash("r_ssrPositionCorrection")
		},
		{
			"r_ssrRoughnessMipParameters",
			"X: mirror mip; Y: roughest mip; Z: roughness middle point, may need different value for different screen resolution on PC.",
			generate_hash("r_ssrRoughnessMipParameters")
		},
		{
			"r_sssBlendWeight",
			"Controls the blend between the wide (zero) and narrow (one) gaussians",
			generate_hash("r_sssBlendWeight")
		},
		{
			"r_sssDebugMaterial",
			"Debug Feature: toggle materials with SSS",
			generate_hash("r_sssDebugMaterial")
		},
		{
			"r_sssEnable",
			"Enables the subsurface scattering effect (note that disabling SSS will not prevent the filter from running)",
			generate_hash("r_sssEnable")
		},
		{
			"r_sssGlobalRadius",
			"Controls the global radius (in inches)",
			generate_hash("r_sssGlobalRadius")
		},
		{
			"r_sssJitterRadius",
			"Percentage of the kernel to be jittered",
			generate_hash("r_sssJitterRadius")
		},
		{
			"r_sssNarrowRadius",
			"Controls the narrow Gaussian radius",
			generate_hash("r_sssNarrowRadius")
		},
		{
			"r_sssPreset",
			"Changes subsurface scattering quality",
			generate_hash("r_sssPreset")
		},
		{
			"r_sssWideRadius",
			"Controls the wide Gaussian radius",
			generate_hash("r_sssWideRadius")
		},
		{
			"r_subdiv",
			"Enables Catmull Clark surface subdivision.",
			generate_hash("r_subdiv")
		},
		{
			"r_subdivLimit",
			"Set the maximum Catmull Clark subdivision level.",
			generate_hash("r_subdivLimit")
		},
		{
			"r_subdivPatchCount",
			"Patches per thread group for sub-division surfaces.",
			generate_hash("r_subdivPatchCount")
		},
		{
			"r_subdomainLimit",
			"Maximum number of extra tessellation subdivisions using instancing (max tess amts are 0:64, 1:128, 2:192, 3:256, max instances used are 0:1, 1:4, 2:9, 3:12)",
			generate_hash("r_subdomainLimit")
		},
		{
			"r_subdomainScale",
			"Debug only: Scales the extra subdivision amount (for values < 1, not all instanced sub triangles will draw).",
			generate_hash("r_subdomainScale")
		},
		{
			"r_subwindow",
			"subwindow to draw: left, right, top, bottom",
			generate_hash("r_subwindow")
		},
		{
			"r_sun_from_dvars",
			"Set sun flare values from dvars rather than the level",
			generate_hash("r_sun_from_dvars")
		},
		{
			"r_sun_fx_position",
			"Position in degrees of the sun effect",
			generate_hash("r_sun_fx_position")
		},
		{
			"r_sunblind_fadein",
			"time in seconds to fade blind from 0% to 100%",
			generate_hash("r_sunblind_fadein")
		},
		{
			"r_sunblind_fadeout",
			"time in seconds to fade blind from 100% to 0%",
			generate_hash("r_sunblind_fadeout")
		},
		{
			"r_sunblind_max_angle",
			"angle from sun in degrees inside which effect is max",
			generate_hash("r_sunblind_max_angle")
		},
		{
			"r_sunblind_max_darken",
			"0-1 fraction for how black the world is at max blind",
			generate_hash("r_sunblind_max_darken")
		},
		{
			"r_sunblind_min_angle",
			"angle from sun in degrees outside which effect is 0",
			generate_hash("r_sunblind_min_angle")
		},
		{
			"r_sunflare_fadein",
			"time in seconds to fade alpha from 0% to 100%",
			generate_hash("r_sunflare_fadein")
		},
		{
			"r_sunflare_fadeout",
			"time in seconds to fade alpha from 100% to 0%",
			generate_hash("r_sunflare_fadeout")
		},
		{
			"r_sunflare_max_alpha",
			"0-1 vertex color and alpha of sun at max effect",
			generate_hash("r_sunflare_max_alpha")
		},
		{
			"r_sunflare_max_angle",
			"angle from sun in degrees inside which effect is max",
			generate_hash("r_sunflare_max_angle")
		},
		{
			"r_sunflare_max_size",
			"largest size of flare effect in pixels at 640x480",
			generate_hash("r_sunflare_max_size")
		},
		{
			"r_sunflare_min_angle",
			"angle from sun in degrees outside which effect is 0",
			generate_hash("r_sunflare_min_angle")
		},
		{
			"r_sunflare_min_size",
			"smallest size of flare effect in pixels at 640x480",
			generate_hash("r_sunflare_min_size")
		},
		{
			"r_sunflare_shader",
			"name for flare effect; can be any material",
			generate_hash("r_sunflare_shader")
		},
		{
			"r_sunglare_fadein",
			"time in seconds to fade glare from 0% to 100%",
			generate_hash("r_sunglare_fadein")
		},
		{
			"r_sunglare_fadeout",
			"time in seconds to fade glare from 100% to 0%",
			generate_hash("r_sunglare_fadeout")
		},
		{
			"r_sunglare_max_angle",
			"angle from sun in degrees inside which effect is max",
			generate_hash("r_sunglare_max_angle")
		},
		{
			"r_sunglare_max_lighten",
			"0-1 fraction for how white the world is at max glare",
			generate_hash("r_sunglare_max_lighten")
		},
		{
			"r_sunglare_min_angle",
			"angle from sun in degrees inside which effect is max",
			generate_hash("r_sunglare_min_angle")
		},
		{
			"r_sunInfDist",
			"Sun infinite distance used to place sun fx",
			generate_hash("r_sunInfDist")
		},
		{
			"r_sunshadowmap_cmdbuf_worker",
			"Process shadowmap command buffer in a separate thread",
			generate_hash("r_sunshadowmap_cmdbuf_worker")
		},
		{
			"r_sunsprite_shader",
			"name for static sprite; can be any material",
			generate_hash("r_sunsprite_shader")
		},
		{
			"r_sunsprite_size",
			"diameter in pixels at 640x480 and 80 fov",
			generate_hash("r_sunsprite_size")
		},
		{
			"r_surfaceHDRScalarUseTweaks",
			"Enables lit and unlit surface scalar tweaks",
			generate_hash("r_surfaceHDRScalarUseTweaks")
		},
		{
			"r_tessellation",
			"Enables tessellation of world geometry, with an optional cutoff distance.",
			generate_hash("r_tessellation")
		},
		{
			"r_tessellationCutoffDistance",
			"Distance at which world geometry ceases to tessellate.",
			generate_hash("r_tessellationCutoffDistance")
		},
		{
			"r_tessellationCutoffFalloff",
			"Range over which tessellation is faded out, up to the cutoff.",
			generate_hash("r_tessellationCutoffFalloff")
		},
		{
			"r_tessellationEyeScale",
			"Scale applied due to eye * object normal for less tessellation on facing polygons.",
			generate_hash("r_tessellationEyeScale")
		},
		{
			"r_tessellationFactor",
			"Target edge length, based on dividing full window height by this factor, for dynamic tessellation.  Use zero to disable tessellation.",
			generate_hash("r_tessellationFactor")
		},
		{
			"r_tessellationHeightAuto",
			"Correctly auto scale displacement heights for layers to grow as texture is stretched over larger surface areas to preserve feature proportions.",
			generate_hash("r_tessellationHeightAuto")
		},
		{
			"r_tessellationHeightScale",
			"Displacement height scale factor.",
			generate_hash("r_tessellationHeightScale")
		},
		{
			"r_tessellationHybrid",
			"Hybrid per pixel displacement scale.",
			generate_hash("r_tessellationHybrid")
		},
		{
			"r_tessellationLodBias",
			"Displacement map lod bias.",
			generate_hash("r_tessellationLodBias")
		},
		{
			"r_texFilterAnisoMax",
			"Maximum anisotropy to use for texture filtering",
			generate_hash("r_texFilterAnisoMax")
		},
		{
			"r_texFilterAnisoMin",
			"Minimum anisotropy to use for texture filtering (overridden by max)",
			generate_hash("r_texFilterAnisoMin")
		},
		{
			"r_texFilterDisable",
			"Disables all texture filtering (uses nearest only.)",
			generate_hash("r_texFilterDisable")
		},
		{
			"r_texFilterMipBias",
			"Change the mipmap bias",
			generate_hash("r_texFilterMipBias")
		},
		{
			"r_texFilterMipMode",
			"Forces all mipmaps to use a particular blend between levels (or disables mipping.)",
			generate_hash("r_texFilterMipMode")
		},
		{
			"r_texFilterProbeBilinear",
			"Force reflection probe to use bilinear filter",
			generate_hash("r_texFilterProbeBilinear")
		},
		{
			"r_texShowMipMode",
			"Forces textures with the specified mip filtering to draw black.",
			generate_hash("r_texShowMipMode")
		},
		{
			"r_thermalColorOffset",
			"Offset of the thermal colors (offset + scale*color)",
			generate_hash("r_thermalColorOffset")
		},
		{
			"r_thermalColorScale",
			"Scale of the thermal colors (offset + scale*color)",
			generate_hash("r_thermalColorScale")
		},
		{
			"r_thermalDetailScale",
			"Scale of the detail that is added to the thermal map from the normal map (multiplies the detail amount from AssetManager)",
			generate_hash("r_thermalDetailScale")
		},
		{
			"r_thermalFadeColor",
			"Color the thermal fades to at distance.",
			generate_hash("r_thermalFadeColor")
		},
		{
			"r_thermalFadeControl",
			"Select thermal fade mode",
			generate_hash("r_thermalFadeControl")
		},
		{
			"r_thermalFadeMax",
			"Distance at which thermal stops fading",
			generate_hash("r_thermalFadeMax")
		},
		{
			"r_thermalFadeMin",
			"Distance at which thermal starts fading",
			generate_hash("r_thermalFadeMin")
		},
		{
			"r_tonemap",
			"HDR Tonemapping mode",
			generate_hash("r_tonemap")
		},
		{
			"r_tonemapAdaptSpeed",
			"HDR Tonemap exposure adaptation speed",
			generate_hash("r_tonemapAdaptSpeed")
		},
		{
			"r_tonemapAuto",
			"HDR Tonemapping performs auto-exposure",
			generate_hash("r_tonemapAuto")
		},
		{
			"r_tonemapAutoExposureAdjust",
			"HDR Tonemap Auto Exposure Adjust value (set to 0.0 for automatic adjustment)",
			generate_hash("r_tonemapAutoExposureAdjust")
		},
		{
			"r_tonemapBlack",
			"HDR Filmic Tonemap black point",
			generate_hash("r_tonemapBlack")
		},
		{
			"r_tonemapBlend",
			"HDR Tonemapping blends between exposures",
			generate_hash("r_tonemapBlend")
		},
		{
			"r_tonemapCrossover",
			"HDR Filmic Tonemap crossover point",
			generate_hash("r_tonemapCrossover")
		},
		{
			"r_tonemapDarkEv",
			"HDR Tonemap Dark EV",
			generate_hash("r_tonemapDarkEv")
		},
		{
			"r_tonemapDarkExposureAdjust",
			"HDR Tonemap Dark Exposure Adjust",
			generate_hash("r_tonemapDarkExposureAdjust")
		},
		{
			"r_tonemapExposure",
			"HDR Tonemap exposure (in EV) override (only works in non-auto mode)",
			generate_hash("r_tonemapExposure")
		},
		{
			"r_tonemapExposureAdjust",
			"HDR Tonemap exposure adjustment (in EV, 0 is no adjustment, works like a camera where +1 reduces EV by 1)",
			generate_hash("r_tonemapExposureAdjust")
		},
		{
			"r_tonemapGamma",
			"HDR Tonemap gamma curve power",
			generate_hash("r_tonemapGamma")
		},
		{
			"r_tonemapHighlightRange",
			"HDR Tonemap dynamic range, which determines white point luminance",
			generate_hash("r_tonemapHighlightRange")
		},
		{
			"r_tonemapLightEv",
			"HDR Tonemap Light EV",
			generate_hash("r_tonemapLightEv")
		},
		{
			"r_tonemapLightExposureAdjust",
			"HDR Tonemap Light Exposure Adjust",
			generate_hash("r_tonemapLightExposureAdjust")
		},
		{
			"r_tonemapLockAutoExposureAdjust",
			"HDR Tonemapping lock auto exposure adjust",
			generate_hash("r_tonemapLockAutoExposureAdjust")
		},
		{
			"r_tonemapMaxExposure",
			"HDR Tonemap maximum exposure (in EV)",
			generate_hash("r_tonemapMaxExposure")
		},
		{
			"r_tonemapMaxExposureAdjust",
			"HDR Tonemap Max Exposure Adjust",
			generate_hash("r_tonemapMaxExposureAdjust")
		},
		{
			"r_tonemapMidEv",
			"HDR Tonemap Mid EV",
			generate_hash("r_tonemapMidEv")
		},
		{
			"r_tonemapMidExposureAdjust",
			"HDR Tonemap Mid Exposure Adjust",
			generate_hash("r_tonemapMidExposureAdjust")
		},
		{
			"r_tonemapMinExposureAdjust",
			"HDR Tonemap Min Exposure Adjust",
			generate_hash("r_tonemapMinExposureAdjust")
		},
		{
			"r_tonemapShoulder",
			"HDR Filmic Tonemap shoulder control (0 is linear)",
			generate_hash("r_tonemapShoulder")
		},
		{
			"r_tonemapToe",
			"HDR Filmic Tonemap toe control  (0 is linear)",
			generate_hash("r_tonemapToe")
		},
		{
			"r_tonemapUseCS",
			"HDR Tonemapping uses compute shader.",
			generate_hash("r_tonemapUseCS")
		},
		{
			"r_tonemapUseTweaks",
			"Override tone map LightSet settings with tweak dvar values.",
			generate_hash("r_tonemapUseTweaks")
		},
		{
			"r_tonemapWhite",
			"HDR Filmic Tonemap white point",
			generate_hash("r_tonemapWhite")
		},
		{
			"r_ui3d_debug_display",
			"Show UI3D debug overlay",
			generate_hash("r_ui3d_debug_display")
		},
		{
			"r_ui3d_h",
			"ui3d texture window height",
			generate_hash("r_ui3d_h")
		},
		{
			"r_ui3d_use_debug_values",
			"Use UI debug values",
			generate_hash("r_ui3d_use_debug_values")
		},
		{
			"r_ui3d_w",
			"ui3d texture window width",
			generate_hash("r_ui3d_w")
		},
		{
			"r_ui3d_x",
			"ui3d texture window x",
			generate_hash("r_ui3d_x")
		},
		{
			"r_ui3d_y",
			"ui3d texture window y",
			generate_hash("r_ui3d_y")
		},
		{
			"r_uiBlurDstMode",
			"UI blur distortion mode. Fast uses the scene mip map render target, PostSun uses a downsampled post sun resolve buffer, PostSun HQ uses a gaussian blurred post sun resolve buffer.",
			generate_hash("r_uiBlurDstMode")
		},
		{
			"r_umbra",
			"Enables Umbra-based portal culling.",
			generate_hash("r_umbra")
		},
		{
			"r_umbraAccurateOcclusionThreshold",
			"The distance (in inches) to which accurate occlusion information is gathered. -1.0 = deduced automatically.",
			generate_hash("r_umbraAccurateOcclusionThreshold")
		},
		{
			"r_umbraExclusive",
			"Toggle Umbra for exclusive static culling (disables static portal dpvs)",
			generate_hash("r_umbraExclusive")
		},
		{
			"r_umbraQueryParts",
			"The number of parts the Umbra query frustum is broken into for async query processing as an M x N grid (0, 0 = all queries are synchronous).",
			generate_hash("r_umbraQueryParts")
		},
		{
			"r_umbraUseBadPlaces",
			"Enable/disable ability to disable umbra when inside special volumes defined in mp/umbraBadPlaces.csv.",
			generate_hash("r_umbraUseBadPlaces")
		},
		{
			"r_umbraUseDpvsCullDist",
			"Use cull distance from the DPVS instead of the far plane distance.",
			generate_hash("r_umbraUseDpvsCullDist")
		},
		{
			"r_unlitSurfaceHDRScalar",
			"Vision set based scalar applied to unlit surfaces to balance those surfaces with the luminance of the scene",
			generate_hash("r_unlitSurfaceHDRScalar")
		},
		{
			"r_useComputeSkinning",
			"Enables compute shader (GPU) skinning.",
			generate_hash("r_useComputeSkinning")
		},
		{
			"r_useLayeredMaterials",
			"Set to true to use layered materials on shader model 3 hardware",
			generate_hash("r_useLayeredMaterials")
		},
		{
			"r_useLightGridDefaultFXLightingLookup",
			"Enable/disable default fx lighting lookup\n",
			generate_hash("r_useLightGridDefaultFXLightingLookup")
		},
		{
			"r_useLightGridDefaultModelLightingLookup",
			"Enable/disable default model lighting lookup\n",
			generate_hash("r_useLightGridDefaultModelLightingLookup")
		},
		{
			"r_useShadowGeomOpt",
			"Enable iwRad shadow geometry optimization. It only works when we have the data generated in iwRad.",
			generate_hash("r_useShadowGeomOpt")
		},
		{
			"r_useSunShadowPortals",
			"Enable sun shadow portals when dir light change and not using cached shadow.",
			generate_hash("r_useSunShadowPortals")
		},
		{
			"r_useXAnimIK",
			"Enables IK animation.",
			generate_hash("r_useXAnimIK")
		},
		{
			"r_vc_makelog",
			"Enable logging of light grid points for the vis cache.  1 starts from scratch, 2 appends.",
			generate_hash("r_vc_makelog")
		},
		{
			"r_vc_showlog",
			"Show this many rows of light grid points for the vis cache",
			generate_hash("r_vc_showlog")
		},
		{
			"r_veil",
			"Apply veiling luminance (HDR glow)",
			generate_hash("r_veil")
		},
		{
			"r_veilAntialiasing",
			"Veil antialiasing mode (downsample technique used for first mip).",
			generate_hash("r_veilAntialiasing")
		},
		{
			"r_veilBackgroundStrength",
			"Strength of background when applying veiling luminance (HDR glow)",
			generate_hash("r_veilBackgroundStrength")
		},
		{
			"r_veilFalloffScale1",
			"Controls the size of individual Gaussians (Gaussians 4-6 in XYZ, where Gaussian 6 is the wider one)",
			generate_hash("r_veilFalloffScale1")
		},
		{
			"r_veilFalloffScale2",
			"Controls the size of individual Gaussians (Gaussians 4-6 in XYZ, where Gaussian 6 is the wider one)",
			generate_hash("r_veilFalloffScale2")
		},
		{
			"r_veilFalloffWeight1",
			"Controls the weight of individual Gaussians (Gaussians 4-6 in XYZ, where Gaussian 6 is the wider one)",
			generate_hash("r_veilFalloffWeight1")
		},
		{
			"r_veilFalloffWeight2",
			"Controls the weight of individual Gaussians (Gaussians 4-6 in XYZ, where Gaussian 6 is the wider one)",
			generate_hash("r_veilFalloffWeight2")
		},
		{
			"r_veilFilter",
			"Changes the veil filtering mode",
			generate_hash("r_veilFilter")
		},
		{
			"r_veilPreset",
			"Changes veil sampling quality",
			generate_hash("r_veilPreset")
		},
		{
			"r_veilRadius",
			"Controls the radius of the first Gaussian in virtual pixels (remaining Gaussians follow proportionally).",
			generate_hash("r_veilRadius")
		},
		{
			"r_veilStrength",
			"Strength of veiling luminance (HDR glow)",
			generate_hash("r_veilStrength")
		},
		{
			"r_veilUseTweaks",
			"Override veil LightSet settings with tweak dvar values.",
			generate_hash("r_veilUseTweaks")
		},
		{
			"r_velocityPrepass",
			"Perform velocity rendering during the depth prepass",
			generate_hash("r_velocityPrepass")
		},
		{
			"r_viewModelLightAmbient",
			"",
			generate_hash("r_viewModelLightAmbient")
		},
		{
			"r_viewModelPrimaryLightTweakDiffuseStrength",
			"Tweak the diffuse intensity for view model primary lights",
			generate_hash("r_viewModelPrimaryLightTweakDiffuseStrength")
		},
		{
			"r_viewModelPrimaryLightTweakSpecularStrength",
			"Tweak the specular intensity for view model primary lights",
			generate_hash("r_viewModelPrimaryLightTweakSpecularStrength")
		},
		{
			"r_viewModelPrimaryLightUseTweaks",
			"",
			generate_hash("r_viewModelPrimaryLightUseTweaks")
		},
		{
			"r_volumeLightScatter",
			"Enables volumetric light scattering",
			generate_hash("r_volumeLightScatter")
		},
		{
			"r_volumeLightScatterAngularAtten",
			"Distance of sun from center of screen before angular attenuation starts for god rays",
			generate_hash("r_volumeLightScatterAngularAtten")
		},
		{
			"r_volumeLightScatterBackgroundDistance",
			"Distance at which pixels are considered background for volume light scatter effect",
			generate_hash("r_volumeLightScatterBackgroundDistance")
		},
		{
			"r_volumeLightScatterColor",
			"",
			generate_hash("r_volumeLightScatterColor")
		},
		{
			"r_volumeLightScatterDepthAttenFar",
			"Pixels >= than this depth recieve full volume light scatter.",
			generate_hash("r_volumeLightScatterDepthAttenFar")
		},
		{
			"r_volumeLightScatterDepthAttenNear",
			"Pixels <= than this depth recieve no volume light scatter.",
			generate_hash("r_volumeLightScatterDepthAttenNear")
		},
		{
			"r_volumeLightScatterEv",
			"Light intensity (in EV) for volumetric light scattering",
			generate_hash("r_volumeLightScatterEv")
		},
		{
			"r_volumeLightScatterLinearAtten",
			"Coefficient of linear attenuation of god rays",
			generate_hash("r_volumeLightScatterLinearAtten")
		},
		{
			"r_volumeLightScatterQuadraticAtten",
			"Coefficient of quadratic attenuation of god rays)",
			generate_hash("r_volumeLightScatterQuadraticAtten")
		},
		{
			"r_volumeLightScatterUseTweaks",
			"Enables volumetric light scattering tweaks",
			generate_hash("r_volumeLightScatterUseTweaks")
		},
		{
			"r_vsync",
			"Enable v-sync before drawing the next frame to avoid 'tearing' artifacts.",
			generate_hash("r_vsync")
		},
		{
			"r_warningRepeatDelay",
			"Number of seconds after displaying a \"per-frame\" warning before it will display again",
			generate_hash("r_warningRepeatDelay")
		},
		{
			"r_wideTessFactorsThreshold",
			"If a surface has more than this many triangles, process triangles in parallel instead of surfaces.",
			generate_hash("r_wideTessFactorsThreshold")
		},
		{
			"r_zfar",
			"Change the distance at which culling fog reaches 100% opacity; 0 is off",
			generate_hash("r_zfar")
		},
		{
			"r_znear",
			"Things closer than this aren't drawn.  Reducing this increases z-fighting in the distance.",
			generate_hash("r_znear")
		},
		{
			"radarjamDistMax",
			"",
			generate_hash("radarjamDistMax")
		},
		{
			"radarjamDistMin",
			"",
			generate_hash("radarjamDistMin")
		},
		{
			"radarjamSinCurve",
			"",
			generate_hash("radarjamSinCurve")
		},
		{
			"radius_damage_debug",
			"Turn on debug lines for radius damage traces",
			generate_hash("radius_damage_debug")
		},
		{
			"ragdoll_baselerp_time",
			"Default time ragdoll baselerp bones take to reach the base pose",
			generate_hash("ragdoll_baselerp_time")
		},
		{
			"ragdoll_bullet_force",
			"Bullet force applied to ragdolls",
			generate_hash("ragdoll_bullet_force")
		},
		{
			"ragdoll_bullet_upbias",
			"Upward bias applied to ragdoll bullet effects",
			generate_hash("ragdoll_bullet_upbias")
		},
		{
			"ragdoll_dump_anims",
			"Dump animation data when ragdoll fails",
			generate_hash("ragdoll_dump_anims")
		},
		{
			"ragdoll_enable",
			"Turn on ragdoll death animations",
			generate_hash("ragdoll_enable")
		},
		{
			"ragdoll_explode_force",
			"Explosive force applied to ragdolls",
			generate_hash("ragdoll_explode_force")
		},
		{
			"ragdoll_explode_upbias",
			"Upwards bias applied to ragdoll explosion effects",
			generate_hash("ragdoll_explode_upbias")
		},
		{
			"ragdoll_exploding_bullet_force",
			"Force applied to ragdolls from explosive bullets",
			generate_hash("ragdoll_exploding_bullet_force")
		},
		{
			"ragdoll_exploding_bullet_upbias",
			"Upwards bias applied to ragdoll from explosive bullets",
			generate_hash("ragdoll_exploding_bullet_upbias")
		},
		{
			"ragdoll_idle_min_velsq",
			"Minimum squared speed a ragdoll body needs to be moving before it will shut down due to time",
			generate_hash("ragdoll_idle_min_velsq")
		},
		{
			"ragdoll_jitter_scale",
			"Scale up or down the effect of physics jitter on ragdolls",
			generate_hash("ragdoll_jitter_scale")
		},
		{
			"ragdoll_jointlerp_time",
			"Default time taken to lerp down ragdoll joint friction",
			generate_hash("ragdoll_jointlerp_time")
		},
		{
			"ragdoll_link_to_moving_platform",
			"Enable client-side linking of ragdolls to script brush models when they go idle.",
			generate_hash("ragdoll_link_to_moving_platform")
		},
		{
			"ragdoll_max_life",
			"Max lifetime of a ragdoll system in msec",
			generate_hash("ragdoll_max_life")
		},
		{
			"ragdoll_max_simulating",
			"Max number of simultaneous active ragdolls - archived",
			generate_hash("ragdoll_max_simulating")
		},
		{
			"ragdoll_max_stretch_pct",
			"Force ragdoll limbs to not stretch more than this percentage in one frame",
			generate_hash("ragdoll_max_stretch_pct")
		},
		{
			"ragdoll_mp_limit",
			"Max number of simultaneous active ragdolls - archived",
			generate_hash("ragdoll_mp_limit")
		},
		{
			"ragdoll_mp_resume_share_after_killcam",
			"Msec after returning from killcam that splitscreen players will share ragdolls again.",
			generate_hash("ragdoll_mp_resume_share_after_killcam")
		},
		{
			"ragdoll_resolve_penetration_bias",
			"Bias value on force to push ragdolls out of environment.",
			generate_hash("ragdoll_resolve_penetration_bias")
		},
		{
			"ragdoll_rotvel_scale",
			"Ragdoll rotational velocity estimate scale",
			generate_hash("ragdoll_rotvel_scale")
		},
		{
			"ragdoll_self_collision_scale",
			"Scale the size of the collision capsules used to prevent ragdoll limbs from interpenetrating",
			generate_hash("ragdoll_self_collision_scale")
		},
		{
			"ragdoll_stretch_iters",
			"Iterations to run the alternate limb solver",
			generate_hash("ragdoll_stretch_iters")
		},
		{
			"rankedPlayEndMatchKeepLobby",
			"keep the lobby if the lobby host is in our private party.",
			generate_hash("rankedPlayEndMatchKeepLobby")
		},
		{
			"rankedPlaylistLockoutDuration",
			"Time in seconds to lock the ranked play playlist if a player quit the match early.",
			generate_hash("rankedPlaylistLockoutDuration")
		},
		{
			"rate",
			"Player's preferred network rate",
			generate_hash("rate")
		},
		{
			"RemoteCameraSounds_DryLevel",
			"",
			generate_hash("RemoteCameraSounds_DryLevel")
		},
		{
			"RemoteCameraSounds_RoomType",
			"",
			generate_hash("RemoteCameraSounds_RoomType")
		},
		{
			"RemoteCameraSounds_WetLevel",
			"",
			generate_hash("RemoteCameraSounds_WetLevel")
		},
		{
			"requireOpenNat",
			"",
			generate_hash("requireOpenNat")
		},
		{
			"restrictMapPacksToGroups",
			"Restrict map pack usage to needing all maps in an ala carte package in order to use as search criteria",
			generate_hash("restrictMapPacksToGroups")
		},
		{
			"riotshield_bullet_damage_scale",
			"Value to scale bullet damage to deployed riotshield.",
			generate_hash("riotshield_bullet_damage_scale")
		},
		{
			"riotshield_deploy_limit_radius",
			"Min distance deployed riotshields must be from each other.",
			generate_hash("riotshield_deploy_limit_radius")
		},
		{
			"riotshield_deploy_trace_parallel",
			"Report collisions when riotshield traces are parallel to plane of triangle.  If disabled traces parallel to triangle planes do not report collisions at all.",
			generate_hash("riotshield_deploy_trace_parallel")
		},
		{
			"riotshield_deployed_health",
			"Deployed riotshield health.",
			generate_hash("riotshield_deployed_health")
		},
		{
			"riotshield_destroyed_cleanup_time",
			"Time (in seconds) destroyed riotshield model persists before disappearing",
			generate_hash("riotshield_destroyed_cleanup_time")
		},
		{
			"riotshield_explosive_damage_scale",
			"Value to scale explosive damage to deployed riotshield..",
			generate_hash("riotshield_explosive_damage_scale")
		},
		{
			"riotshield_melee_damage_scale",
			"Value to scale melee damage to deployed riotshield.",
			generate_hash("riotshield_melee_damage_scale")
		},
		{
			"riotshield_projectile_damage_scale",
			"Value to scale projectile damage to deployed riotshield.",
			generate_hash("riotshield_projectile_damage_scale")
		},
		{
			"s_aggregate_ping_offset",
			"offset to apply to aggregate ping values",
			generate_hash("s_aggregate_ping_offset")
		},
		{
			"s_aggregate_ping_scale",
			"8-bit fixed-point aggregate ping scaler value",
			generate_hash("s_aggregate_ping_scale")
		},
		{
			"s_avg_max_weighting",
			"weighting from 0-256 of party average ping vs. worst ping",
			generate_hash("s_avg_max_weighting")
		},
		{
			"s_ds_pingclient_reping_wait_db",
			"wait this# of frames for the db thread to settle down before repinging",
			generate_hash("s_ds_pingclient_reping_wait_db")
		},
		{
			"s_use_aggregate_datacenter_pings",
			"use newer system for aggregating party pings",
			generate_hash("s_use_aggregate_datacenter_pings")
		},
		{
			"safeArea_adjusted_horizontal",
			"User-adjustable horizontal safe area as a fraction of the screen width",
			generate_hash("safeArea_adjusted_horizontal")
		},
		{
			"safeArea_adjusted_vertical",
			"User-adjustable vertical safe area as a fraction of the screen height",
			generate_hash("safeArea_adjusted_vertical")
		},
		{
			"safeArea_horizontal",
			"Horizontal safe area as a fraction of the screen width",
			generate_hash("safeArea_horizontal")
		},
		{
			"safeArea_vertical",
			"Vertical safe area as a fraction of the screen height",
			generate_hash("safeArea_vertical")
		},
		{
			"scr_conf_numlives",
			"",
			generate_hash("scr_conf_numlives")
		},
		{
			"scr_conf_playerrespawndelay",
			"",
			generate_hash("scr_conf_playerrespawndelay")
		},
		{
			"scr_conf_roundlimit",
			"",
			generate_hash("scr_conf_roundlimit")
		},
		{
			"scr_conf_scorelimit",
			"",
			generate_hash("scr_conf_scorelimit")
		},
		{
			"scr_conf_timelimit",
			"",
			generate_hash("scr_conf_timelimit")
		},
		{
			"scr_conf_waverespawndelay",
			"",
			generate_hash("scr_conf_waverespawndelay")
		},
		{
			"scr_conf_winlimit",
			"",
			generate_hash("scr_conf_winlimit")
		},
		{
			"scr_default_maxagents",
			"",
			generate_hash("scr_default_maxagents")
		},
		{
			"scr_diehard",
			"",
			generate_hash("scr_diehard")
		},
		{
			"scr_disableClientSpawnTraces",
			"",
			generate_hash("scr_disableClientSpawnTraces")
		},
		{
			"scr_dm_numlives",
			"",
			generate_hash("scr_dm_numlives")
		},
		{
			"scr_dm_playerrespawndelay",
			"",
			generate_hash("scr_dm_playerrespawndelay")
		},
		{
			"scr_dm_roundlimit",
			"",
			generate_hash("scr_dm_roundlimit")
		},
		{
			"scr_dm_scorelimit",
			"",
			generate_hash("scr_dm_scorelimit")
		},
		{
			"scr_dm_timelimit",
			"",
			generate_hash("scr_dm_timelimit")
		},
		{
			"scr_dm_waverespawndelay",
			"",
			generate_hash("scr_dm_waverespawndelay")
		},
		{
			"scr_dm_winlimit",
			"",
			generate_hash("scr_dm_winlimit")
		},
		{
			"scr_dom_numlives",
			"",
			generate_hash("scr_dom_numlives")
		},
		{
			"scr_dom_playerrespawndelay",
			"",
			generate_hash("scr_dom_playerrespawndelay")
		},
		{
			"scr_dom_roundlimit",
			"",
			generate_hash("scr_dom_roundlimit")
		},
		{
			"scr_dom_scorelimit",
			"",
			generate_hash("scr_dom_scorelimit")
		},
		{
			"scr_dom_timelimit",
			"",
			generate_hash("scr_dom_timelimit")
		},
		{
			"scr_dom_waverespawndelay",
			"",
			generate_hash("scr_dom_waverespawndelay")
		},
		{
			"scr_dom_winlimit",
			"",
			generate_hash("scr_dom_winlimit")
		},
		{
			"scr_explBulletMod",
			"",
			generate_hash("scr_explBulletMod")
		},
		{
			"scr_game_allowkillcam",
			"script allow killcam",
			generate_hash("scr_game_allowkillcam")
		},
		{
			"scr_game_deathpointloss",
			"",
			generate_hash("scr_game_deathpointloss")
		},
		{
			"scr_game_forceuav",
			"",
			generate_hash("scr_game_forceuav")
		},
		{
			"scr_game_graceperiod",
			"",
			generate_hash("scr_game_graceperiod")
		},
		{
			"scr_game_hardpoints",
			"",
			generate_hash("scr_game_hardpoints")
		},
		{
			"scr_game_killstreakdelay",
			"",
			generate_hash("scr_game_killstreakdelay")
		},
		{
			"scr_game_lockspectatorpov",
			"Lock spectator mode globally, 0=freelook/unlocked, 1=first_person, 2=third_person",
			generate_hash("scr_game_lockspectatorpov")
		},
		{
			"scr_game_onlyheadshots",
			"",
			generate_hash("scr_game_onlyheadshots")
		},
		{
			"scr_game_perks",
			"",
			generate_hash("scr_game_perks")
		},
		{
			"scr_game_spectatetype",
			"",
			generate_hash("scr_game_spectatetype")
		},
		{
			"scr_game_suicidepointloss",
			"",
			generate_hash("scr_game_suicidepointloss")
		},
		{
			"scr_gameended",
			"",
			generate_hash("scr_gameended")
		},
		{
			"scr_hardcore",
			"",
			generate_hash("scr_hardcore")
		},
		{
			"scr_horde_difficulty",
			"",
			generate_hash("scr_horde_difficulty")
		},
		{
			"scr_horde_maxagents",
			"",
			generate_hash("scr_horde_maxagents")
		},
		{
			"scr_horde_numlives",
			"",
			generate_hash("scr_horde_numlives")
		},
		{
			"scr_horde_playerrespawndelay",
			"",
			generate_hash("scr_horde_playerrespawndelay")
		},
		{
			"scr_horde_roundlimit",
			"",
			generate_hash("scr_horde_roundlimit")
		},
		{
			"scr_horde_scorelimit",
			"",
			generate_hash("scr_horde_scorelimit")
		},
		{
			"scr_horde_timelimit",
			"",
			generate_hash("scr_horde_timelimit")
		},
		{
			"scr_horde_waverespawndelay",
			"",
			generate_hash("scr_horde_waverespawndelay")
		},
		{
			"scr_horde_winlimit",
			"",
			generate_hash("scr_horde_winlimit")
		},
		{
			"scr_infect_numlives",
			"",
			generate_hash("scr_infect_numlives")
		},
		{
			"scr_infect_playerrespawndelay",
			"",
			generate_hash("scr_infect_playerrespawndelay")
		},
		{
			"scr_infect_roundlimit",
			"",
			generate_hash("scr_infect_roundlimit")
		},
		{
			"scr_infect_timelimit",
			"",
			generate_hash("scr_infect_timelimit")
		},
		{
			"scr_infect_waverespawndelay",
			"",
			generate_hash("scr_infect_waverespawndelay")
		},
		{
			"scr_infect_winlimit",
			"",
			generate_hash("scr_infect_winlimit")
		},
		{
			"scr_isgamescom",
			"script use gamescom demo flow",
			generate_hash("scr_isgamescom")
		},
		{
			"scr_maxPerPlayerExplosives",
			"",
			generate_hash("scr_maxPerPlayerExplosives")
		},
		{
			"scr_nukeCancelMode",
			"",
			generate_hash("scr_nukeCancelMode")
		},
		{
			"scr_nukeTimer",
			"",
			generate_hash("scr_nukeTimer")
		},
		{
			"scr_patientZero",
			"",
			generate_hash("scr_patientZero")
		},
		{
			"scr_player_forcerespawn",
			"",
			generate_hash("scr_player_forcerespawn")
		},
		{
			"scr_player_healthregentime",
			"",
			generate_hash("scr_player_healthregentime")
		},
		{
			"scr_player_maxhealth",
			"",
			generate_hash("scr_player_maxhealth")
		},
		{
			"scr_player_numlives",
			"",
			generate_hash("scr_player_numlives")
		},
		{
			"scr_player_respawndelay",
			"",
			generate_hash("scr_player_respawndelay")
		},
		{
			"scr_player_sprinttime",
			"",
			generate_hash("scr_player_sprinttime")
		},
		{
			"scr_player_suicidespawndelay",
			"",
			generate_hash("scr_player_suicidespawndelay")
		},
		{
			"scr_RequiredMapAspectratio",
			"",
			generate_hash("scr_RequiredMapAspectratio")
		},
		{
			"scr_riotShieldXPBullets",
			"",
			generate_hash("scr_riotShieldXPBullets")
		},
		{
			"scr_sd_bombtimer",
			"",
			generate_hash("scr_sd_bombtimer")
		},
		{
			"scr_sd_defusetime",
			"",
			generate_hash("scr_sd_defusetime")
		},
		{
			"scr_sd_multibomb",
			"",
			generate_hash("scr_sd_multibomb")
		},
		{
			"scr_sd_numlives",
			"",
			generate_hash("scr_sd_numlives")
		},
		{
			"scr_sd_planttime",
			"",
			generate_hash("scr_sd_planttime")
		},
		{
			"scr_sd_playerrespawndelay",
			"",
			generate_hash("scr_sd_playerrespawndelay")
		},
		{
			"scr_sd_roundlimit",
			"",
			generate_hash("scr_sd_roundlimit")
		},
		{
			"scr_sd_roundswitch",
			"",
			generate_hash("scr_sd_roundswitch")
		},
		{
			"scr_sd_scorelimit",
			"",
			generate_hash("scr_sd_scorelimit")
		},
		{
			"scr_sd_timelimit",
			"",
			generate_hash("scr_sd_timelimit")
		},
		{
			"scr_sd_waverespawndelay",
			"",
			generate_hash("scr_sd_waverespawndelay")
		},
		{
			"scr_sd_winlimit",
			"",
			generate_hash("scr_sd_winlimit")
		},
		{
			"scr_sr_bombtimer",
			"",
			generate_hash("scr_sr_bombtimer")
		},
		{
			"scr_sr_defusetime",
			"",
			generate_hash("scr_sr_defusetime")
		},
		{
			"scr_sr_multibomb",
			"",
			generate_hash("scr_sr_multibomb")
		},
		{
			"scr_sr_numlives",
			"",
			generate_hash("scr_sr_numlives")
		},
		{
			"scr_sr_planttime",
			"",
			generate_hash("scr_sr_planttime")
		},
		{
			"scr_sr_playerrespawndelay",
			"",
			generate_hash("scr_sr_playerrespawndelay")
		},
		{
			"scr_sr_roundlimit",
			"",
			generate_hash("scr_sr_roundlimit")
		},
		{
			"scr_sr_roundswitch",
			"",
			generate_hash("scr_sr_roundswitch")
		},
		{
			"scr_sr_scorelimit",
			"",
			generate_hash("scr_sr_scorelimit")
		},
		{
			"scr_sr_timelimit",
			"",
			generate_hash("scr_sr_timelimit")
		},
		{
			"scr_sr_waverespawndelay",
			"",
			generate_hash("scr_sr_waverespawndelay")
		},
		{
			"scr_sr_winlimit",
			"",
			generate_hash("scr_sr_winlimit")
		},
		{
			"scr_team_fftype",
			"script team friendly fire type",
			generate_hash("scr_team_fftype")
		},
		{
			"scr_team_respawntime",
			"",
			generate_hash("scr_team_respawntime")
		},
		{
			"scr_team_teamkillpointloss",
			"",
			generate_hash("scr_team_teamkillpointloss")
		},
		{
			"scr_team_teamkillspawndelay",
			"",
			generate_hash("scr_team_teamkillspawndelay")
		},
		{
			"scr_thirdPerson",
			"",
			generate_hash("scr_thirdPerson")
		},
		{
			"scr_tispawndelay",
			"",
			generate_hash("scr_tispawndelay")
		},
		{
			"scr_war_halftime",
			"",
			generate_hash("scr_war_halftime")
		},
		{
			"scr_war_numlives",
			"",
			generate_hash("scr_war_numlives")
		},
		{
			"scr_war_playerrespawndelay",
			"",
			generate_hash("scr_war_playerrespawndelay")
		},
		{
			"scr_war_roundlimit",
			"",
			generate_hash("scr_war_roundlimit")
		},
		{
			"scr_war_roundswitch",
			"",
			generate_hash("scr_war_roundswitch")
		},
		{
			"scr_war_scorelimit",
			"",
			generate_hash("scr_war_scorelimit")
		},
		{
			"scr_war_timelimit",
			"",
			generate_hash("scr_war_timelimit")
		},
		{
			"scr_war_waverespawndelay",
			"",
			generate_hash("scr_war_waverespawndelay")
		},
		{
			"scr_war_winlimit",
			"",
			generate_hash("scr_war_winlimit")
		},
		{
			"scr_xpscale",
			"",
			generate_hash("scr_xpscale")
		},
		{
			"screenshots_active",
			"Are we allowed to enable Screenshots or not",
			generate_hash("screenshots_active")
		},
		{
			"search_weight_asn",
			"The weight used for the asn in weighted matchmaking.",
			generate_hash("search_weight_asn")
		},
		{
			"search_weight_country_code",
			"The weight used for the country code in weighted matchmaking.",
			generate_hash("search_weight_country_code")
		},
		{
			"search_weight_lat_long",
			"The weight used for the lat long in weighted matchmaking.",
			generate_hash("search_weight_lat_long")
		},
		{
			"sensitivity",
			"Mouse sensitivity",
			generate_hash("sensitivity")
		},
		{
			"sentry_placement_feet_offset",
			"Position of the feet from the center axis.",
			generate_hash("sentry_placement_feet_offset")
		},
		{
			"sentry_placement_feet_trace_dist_z",
			"Max distance for a foot to be considered touching the ground",
			generate_hash("sentry_placement_feet_trace_dist_z")
		},
		{
			"sentry_placement_trace_dist",
			"Distance along the trace axis where the sentry will attempt to position itself",
			generate_hash("sentry_placement_trace_dist")
		},
		{
			"sentry_placement_trace_min_normal",
			"Minimum normal to accept a sentry position",
			generate_hash("sentry_placement_trace_min_normal")
		},
		{
			"sentry_placement_trace_parallel",
			"Enable turret traces that are parallel to plane of triangle.  If 0, traces parallel to triangle planes do not report collisions at all.  If 2 (debug-only), then trace code ping pongs between new and old.",
			generate_hash("sentry_placement_trace_parallel")
		},
		{
			"sentry_placement_trace_pitch",
			"Pitch used for the trace axis",
			generate_hash("sentry_placement_trace_pitch")
		},
		{
			"sentry_placement_trace_radius",
			"Radius of the bound used for the placement trace",
			generate_hash("sentry_placement_trace_radius")
		},
		{
			"sentry_placement_trace_radius_canon_safety",
			"Extra radius used in the forward direction to compensate for the canon length",
			generate_hash("sentry_placement_trace_radius_canon_safety")
		},
		{
			"server1",
			"",
			generate_hash("server1")
		},
		{
			"server10",
			"",
			generate_hash("server10")
		},
		{
			"server11",
			"",
			generate_hash("server11")
		},
		{
			"server12",
			"",
			generate_hash("server12")
		},
		{
			"server13",
			"",
			generate_hash("server13")
		},
		{
			"server14",
			"",
			generate_hash("server14")
		},
		{
			"server15",
			"",
			generate_hash("server15")
		},
		{
			"server16",
			"",
			generate_hash("server16")
		},
		{
			"server2",
			"",
			generate_hash("server2")
		},
		{
			"server3",
			"",
			generate_hash("server3")
		},
		{
			"server4",
			"",
			generate_hash("server4")
		},
		{
			"server5",
			"",
			generate_hash("server5")
		},
		{
			"server6",
			"",
			generate_hash("server6")
		},
		{
			"server7",
			"",
			generate_hash("server7")
		},
		{
			"server8",
			"",
			generate_hash("server8")
		},
		{
			"server9",
			"",
			generate_hash("server9")
		},
		{
			"session_immediateDeleteTinySessions",
			"Whether to immediately delete sessions with 1 user",
			generate_hash("session_immediateDeleteTinySessions")
		},
		{
			"session_modify_retry_on_failure",
			"Enable session modify retry on failures.",
			generate_hash("session_modify_retry_on_failure")
		},
		{
			"session_nonblocking",
			"Non-blocking Session code",
			generate_hash("session_nonblocking")
		},
		{
			"sf_use_bw",
			"Intel Cheat - CoD Noir.",
			generate_hash("sf_use_bw")
		},
		{
			"sf_use_chaplin",
			"Intel Cheat - Ragtime Warfare.",
			generate_hash("sf_use_chaplin")
		},
		{
			"sf_use_clustergrenade",
			"Intel Cheat - Cluster Bombs.",
			generate_hash("sf_use_clustergrenade")
		},
		{
			"sf_use_contrast",
			"Intel Cheat - Super Contrast.",
			generate_hash("sf_use_contrast")
		},
		{
			"sf_use_ignoreammo",
			"Intel Cheat - Infinite Ammo.",
			generate_hash("sf_use_ignoreammo")
		},
		{
			"sf_use_invert",
			"Intel Cheat - Photo-Negative.",
			generate_hash("sf_use_invert")
		},
		{
			"sf_use_lemonade_mode",
			"Intel Cheat - Lemon-nade.",
			generate_hash("sf_use_lemonade_mode")
		},
		{
			"sf_use_melon_mode",
			"Intel Cheat - Melon Heads.",
			generate_hash("sf_use_melon_mode")
		},
		{
			"sf_use_ragdoll_mode",
			"Intel Cheat - Ragdoll Impact.",
			generate_hash("sf_use_ragdoll_mode")
		},
		{
			"sf_use_slowmo",
			"Intel Cheat - Slow-Mo Ability.",
			generate_hash("sf_use_slowmo")
		},
		{
			"sf_use_tire_explosion",
			"Intel Cheat - A Bad Year.",
			generate_hash("sf_use_tire_explosion")
		},
		{
			"sf_use_tracksuit_mode",
			"Intel Cheat - Zakhaev's Sons.",
			generate_hash("sf_use_tracksuit_mode")
		},
		{
			"shortversion",
			"Short game version",
			generate_hash("shortversion")
		},
		{
			"showDebugAmmoCounter",
			"Show the debug ammo counter when unable to show ar ammo counter",
			generate_hash("showDebugAmmoCounter")
		},
		{
			"showPlaylistTotalPlayers",
			"Toggle the display of the total number of players in a playlist and online",
			generate_hash("showPlaylistTotalPlayers")
		},
		{
			"sm_cacheSpotShadows",
			"Cache spot shadow maps, improves shadow map performance at the cost of memory (requires vid_restart)",
			generate_hash("sm_cacheSpotShadows")
		},
		{
			"sm_cacheSpotShadowsEnabled",
			"Enables caching of spot shadows.",
			generate_hash("sm_cacheSpotShadowsEnabled")
		},
		{
			"sm_cacheSunShadow",
			"Cache sun shadow map, improves shadow map performance at the cost of memory (requires vid_restart)",
			generate_hash("sm_cacheSunShadow")
		},
		{
			"sm_cacheSunShadowEnabled",
			"Enables caching of sun-based shadows.",
			generate_hash("sm_cacheSunShadowEnabled")
		},
		{
			"sm_cameraOffset",
			"",
			generate_hash("sm_cameraOffset")
		},
		{
			"sm_dynlightAllSModels",
			"Enable, from script, rendering all static models in dynamic light volume when shadow mapping",
			generate_hash("sm_dynlightAllSModels")
		},
		{
			"sm_enable",
			"Enable shadow mapping",
			generate_hash("sm_enable")
		},
		{
			"sm_fastSunShadow",
			"Fast sun shadow",
			generate_hash("sm_fastSunShadow")
		},
		{
			"sm_lightScore_eyeProjectDist",
			"When picking shadows for primary lights, measure distance from a point this far in front of the camera.",
			generate_hash("sm_lightScore_eyeProjectDist")
		},
		{
			"sm_lightScore_spotProjectFrac",
			"When picking shadows for primary lights, measure distance from a point this far in front of the camera.",
			generate_hash("sm_lightScore_spotProjectFrac")
		},
		{
			"sm_maxLightsWithShadows",
			"Limits how many primary lights can have shadow maps",
			generate_hash("sm_maxLightsWithShadows")
		},
		{
			"sm_minSpotLightScore",
			"Minimum score (based on intensity, radius, and position relative to the camera) for a spot light to have shadow maps.",
			generate_hash("sm_minSpotLightScore")
		},
		{
			"sm_polygonOffsetBias",
			"Shadow map offset bias",
			generate_hash("sm_polygonOffsetBias")
		},
		{
			"sm_polygonOffsetClamp",
			"Shadow map offset clamp",
			generate_hash("sm_polygonOffsetClamp")
		},
		{
			"sm_polygonOffsetPreset",
			"Shadow map polygon offset preset.",
			generate_hash("sm_polygonOffsetPreset")
		},
		{
			"sm_polygonOffsetScale",
			"Shadow map offset scale",
			generate_hash("sm_polygonOffsetScale")
		},
		{
			"sm_qualitySpotShadow",
			"Fast spot shadow",
			generate_hash("sm_qualitySpotShadow")
		},
		{
			"sm_shadowUseTweaks",
			"Override shadow LightSet settings with tweak dvar values.",
			generate_hash("sm_shadowUseTweaks")
		},
		{
			"sm_spotDistCull",
			"Distance cull spot shadows",
			generate_hash("sm_spotDistCull")
		},
		{
			"sm_spotEnable",
			"Enable spot shadow mapping from script",
			generate_hash("sm_spotEnable")
		},
		{
			"sm_spotFilterRadius",
			"Spot soft shadows filter radius",
			generate_hash("sm_spotFilterRadius")
		},
		{
			"sm_spotLightScoreModelScale",
			"Scale the calculated spot light score by this value if the light currently only affects static or script brush models.",
			generate_hash("sm_spotLightScoreModelScale")
		},
		{
			"sm_spotLightScoreRadiusPower",
			"Power to apply to light radius when determining spot light shadowing score (1.0 means radius scales up score a lot, 0.0 means don't scale score using radius)",
			generate_hash("sm_spotLightScoreRadiusPower")
		},
		{
			"sm_spotLimit",
			"Limit number of spot shadows from script",
			generate_hash("sm_spotLimit")
		},
		{
			"sm_spotShadowFadeTime",
			"How many seconds it takes for a primary light shadow map to fade in or out",
			generate_hash("sm_spotShadowFadeTime")
		},
		{
			"sm_strictCull",
			"Strict shadow map cull",
			generate_hash("sm_strictCull")
		},
		{
			"sm_sunEnable",
			"Enable sun shadow mapping from script",
			generate_hash("sm_sunEnable")
		},
		{
			"sm_sunFilterRadius",
			"Sun soft shadows filter radius",
			generate_hash("sm_sunFilterRadius")
		},
		{
			"sm_sunSampleSizeNear",
			"Shadow sample size",
			generate_hash("sm_sunSampleSizeNear")
		},
		{
			"sm_sunShadowBoundsMax",
			"Max Shadow Bounds",
			generate_hash("sm_sunShadowBoundsMax")
		},
		{
			"sm_sunShadowBoundsMin",
			"Min Shadow Bounds",
			generate_hash("sm_sunShadowBoundsMin")
		},
		{
			"sm_sunShadowBoundsOverride",
			"Override Shadow Bounds",
			generate_hash("sm_sunShadowBoundsOverride")
		},
		{
			"sm_sunShadowCenter",
			"Sun shadow center, 0 0 0 means don't override",
			generate_hash("sm_sunShadowCenter")
		},
		{
			"sm_sunShadowCenterMode",
			"When false center value only used for far map, when true sets both maps",
			generate_hash("sm_sunShadowCenterMode")
		},
		{
			"sm_sunShadowScale",
			"Sun shadow scale optimization",
			generate_hash("sm_sunShadowScale")
		},
		{
			"sm_sunShadowScaleLocked",
			"Lock usage of sm_sunShadowScale at 1",
			generate_hash("sm_sunShadowScaleLocked")
		},
		{
			"sm_usedSunCascadeCount",
			"How many shadow cascade we are using",
			generate_hash("sm_usedSunCascadeCount")
		},
		{
			"snd_allowHeadphoneHRTF",
			"Enable HRTF over headphones",
			generate_hash("snd_allowHeadphoneHRTF")
		},
		{
			"snd_announcerDisabled",
			"Disable all in-game announcers",
			generate_hash("snd_announcerDisabled")
		},
		{
			"snd_announcerVoicePrefix",
			"Local mp announcer voice to use",
			generate_hash("snd_announcerVoicePrefix")
		},
		{
			"snd_battlechatterDisabled",
			"Disable all in-game battle chatter",
			generate_hash("snd_battlechatterDisabled")
		},
		{
			"snd_cinematicVolumeScale",
			"Scales the volume of Bink videos.",
			generate_hash("snd_cinematicVolumeScale")
		},
		{
			"snd_detectedSpeakerConfig",
			"speaker configuration:\n0: autodetect\n1: mono\n2: stereo\n4: quadrophonic\n6: 5.1 surround\n8: 7.1 surround",
			generate_hash("snd_detectedSpeakerConfig")
		},
		{
			"snd_dopplerAuditionEnable",
			"Enables doppler calculation preview mode",
			generate_hash("snd_dopplerAuditionEnable")
		},
		{
			"snd_dopplerBaseSpeedOfSound",
			"The base speed of sound used in doppler calculation",
			generate_hash("snd_dopplerBaseSpeedOfSound")
		},
		{
			"snd_dopplerEnable",
			"Enables doppler calculation",
			generate_hash("snd_dopplerEnable")
		},
		{
			"snd_dopplerPitchMax",
			"Maximum pitch that can be legally applied by doppler",
			generate_hash("snd_dopplerPitchMax")
		},
		{
			"snd_dopplerPitchMin",
			"Minimum pitch that can be legally applied by doppler",
			generate_hash("snd_dopplerPitchMin")
		},
		{
			"snd_dopplerPlayerVelocityScale",
			"The scale of the player velocity, relative the the sound source velocity, when applied to the doppler calculation",
			generate_hash("snd_dopplerPlayerVelocityScale")
		},
		{
			"snd_dopplerSmoothing",
			"Smoothing factor applied to doppler to eliminate jitter or sudden acceleration changes",
			generate_hash("snd_dopplerSmoothing")
		},
		{
			"snd_draw3D",
			"Draw the position and info of world sounds",
			generate_hash("snd_draw3D")
		},
		{
			"snd_drawInfo",
			"Draw debugging information for sounds",
			generate_hash("snd_drawInfo")
		},
		{
			"snd_enable2D",
			"Enable 2D sounds",
			generate_hash("snd_enable2D")
		},
		{
			"snd_enable3D",
			"Enable 3D sounds",
			generate_hash("snd_enable3D")
		},
		{
			"snd_enableEq",
			"Enable equalization filter",
			generate_hash("snd_enableEq")
		},
		{
			"snd_enableReverb",
			"Enable sound reverberation",
			generate_hash("snd_enableReverb")
		},
		{
			"snd_enableStream",
			"Enable streamed sounds",
			generate_hash("snd_enableStream")
		},
		{
			"snd_envFollowerBuffScale",
			"Amount of buffer to use for envelope follower. Smaller value indicates faster envelope.",
			generate_hash("snd_envFollowerBuffScale")
		},
		{
			"snd_errorOnMissing",
			"Cause a Com_Error if a sound file is missing.",
			generate_hash("snd_errorOnMissing")
		},
		{
			"snd_hitsoundDisabled",
			"Disable the hit indicator sound",
			generate_hash("snd_hitsoundDisabled")
		},
		{
			"snd_inheritSecondaryPitchVol",
			"Set to true for secondary aliases to inherit the pitch of the parent",
			generate_hash("snd_inheritSecondaryPitchVol")
		},
		{
			"snd_levelFadeTime",
			"The amout of time in milliseconds for all audio to fade in at the start of a level",
			generate_hash("snd_levelFadeTime")
		},
		{
			"snd_loadFadeTime",
			"Fade time for loading from a checkpoint after death.",
			generate_hash("snd_loadFadeTime")
		},
		{
			"snd_loopFadeTime",
			"Fade-in time for looping sounds.",
			generate_hash("snd_loopFadeTime")
		},
		{
			"snd_musicDisabled",
			"Disable all in-game music",
			generate_hash("snd_musicDisabled")
		},
		{
			"snd_musicDisabledForCustomSoundtrack",
			"Disable all in-game music due to user playing a custom soundtrack",
			generate_hash("snd_musicDisabledForCustomSoundtrack")
		},
		{
			"snd_occlusionDelay",
			"Minimum delay in (ms) between occlusion updates",
			generate_hash("snd_occlusionDelay")
		},
		{
			"snd_occlusionLerpTime",
			"Time to lerp to target occlusion lerp when occluded",
			generate_hash("snd_occlusionLerpTime")
		},
		{
			"snd_peakLimiterCompression",
			"Peak limiter compression factor. The output data is scaled by this and then normalized: F < 1 = disabled; F >= 1 enabled.",
			generate_hash("snd_peakLimiterCompression")
		},
		{
			"snd_peakLimiterDecay",
			"Peak limiter compression decay ratio.",
			generate_hash("snd_peakLimiterDecay")
		},
		{
			"snd_peakLimiterSustainFrames",
			"Number of frames to sustain the limiter peak. 1 frame = 10 msec.",
			generate_hash("snd_peakLimiterSustainFrames")
		},
		{
			"snd_premixVolume",
			"Game sound pre-mix volume",
			generate_hash("snd_premixVolume")
		},
		{
			"snd_reverbZoneOutsideFactor",
			"When a 3d sound is played in a different reverb zone than the player, this factor will be applied to its wet level.",
			generate_hash("snd_reverbZoneOutsideFactor")
		},
		{
			"snd_slaveFadeTime",
			"The amount of time in milliseconds for a 'slave' sound\nto fade its volumes when a master sound starts or stops",
			generate_hash("snd_slaveFadeTime")
		},
		{
			"snd_speakerConfig",
			"speaker configuration:\n0: autodetect\n1: mono\n2: stereo\n4: quadrophonic\n6: 5.1 surround\n8: 7.1 surround",
			generate_hash("snd_speakerConfig")
		},
		{
			"snd_touchStreamFilesOnLoad",
			"Check whether stream sound files exist while loading",
			generate_hash("snd_touchStreamFilesOnLoad")
		},
		{
			"snd_useOldPanning",
			"Use old and busted panning",
			generate_hash("snd_useOldPanning")
		},
		{
			"snd_virtualChannelInfo",
			"Display virtual voice info.",
			generate_hash("snd_virtualChannelInfo")
		},
		{
			"snd_virtualMinDur",
			"The minimum duration (in seconds) of a sound if it is to be added to the virtual voice buffer.",
			generate_hash("snd_virtualMinDur")
		},
		{
			"snd_virtualMinPri",
			"The minimum priority of an alias if it is to be added to the virtual voice buffer.",
			generate_hash("snd_virtualMinPri")
		},
		{
			"snd_virtualMinTimeLeftToRevive",
			"The minimum time (in ms) left in a sample in order to attempt to revive it.",
			generate_hash("snd_virtualMinTimeLeftToRevive")
		},
		{
			"snd_virtualReviveVoices",
			"Whether or not to restore virtual voices.",
			generate_hash("snd_virtualReviveVoices")
		},
		{
			"snd_virtualWaitToReviveTime",
			"The minimum time (in ms) to wait before trying to revive the voice.",
			generate_hash("snd_virtualWaitToReviveTime")
		},
		{
			"snd_volume",
			"Game sound master volume",
			generate_hash("snd_volume")
		},
		{
			"speech_active",
			"Are we allowed to enable Speech or not",
			generate_hash("speech_active")
		},
		{
			"splitscreen",
			"Current game is a splitscreen game",
			generate_hash("splitscreen")
		},
		{
			"steam_ingame_p2p_throttle",
			"Time, in MS, to wait between P2P packet lookups when in-game",
			generate_hash("steam_ingame_p2p_throttle")
		},
		{
			"stringtable_debug",
			"spam debug info for stringtable lookups",
			generate_hash("stringtable_debug")
		},
		{
			"sv_allowClientConsole",
			"Allow remote clients to access the console",
			generate_hash("sv_allowClientConsole")
		},
		{
			"sv_allowedClan1",
			"",
			generate_hash("sv_allowedClan1")
		},
		{
			"sv_allowedClan2",
			"",
			generate_hash("sv_allowedClan2")
		},
		{
			"sv_archiveClientsPositions",
			"Archive the client positions to speed up SV_GetClientPositionsAtTime",
			generate_hash("sv_archiveClientsPositions")
		},
		{
			"sv_checkMinPlayers",
			"Check min players. 0 disables",
			generate_hash("sv_checkMinPlayers")
		},
		{
			"sv_clientArchive",
			"Have the clients archive data to save bandwidth on the server",
			generate_hash("sv_clientArchive")
		},
		{
			"sv_connectTimeout",
			"seconds without any message when a client is loading",
			generate_hash("sv_connectTimeout")
		},
		{
			"sv_cumulThinkTime",
			"Max client think per server 50 msec frame",
			generate_hash("sv_cumulThinkTime")
		},
		{
			"sv_error_on_baseline_failure",
			"Throw an error if the const baseline data is invalid.",
			generate_hash("sv_error_on_baseline_failure")
		},
		{
			"sv_exponentialBackoffAfterNonAckedMsgs",
			"start exponential backoff on msg frequency if the client has not acked the last X messages",
			generate_hash("sv_exponentialBackoffAfterNonAckedMsgs")
		},
		{
			"sv_hostname",
			"Host name of the server",
			generate_hash("sv_hostname")
		},
		{
			"sv_hugeSnapshotDelay",
			"How long to wait before building a new snapshot after a 'huge' snapshot is sent",
			generate_hash("sv_hugeSnapshotDelay")
		},
		{
			"sv_hugeSnapshotSize",
			"Size of a snapshot to be considered 'huge'",
			generate_hash("sv_hugeSnapshotSize")
		},
		{
			"sv_kickBanTime",
			"Time in seconds for a player to be banned from the server after being kicked",
			generate_hash("sv_kickBanTime")
		},
		{
			"sv_local_client_snapshot_msec",
			"Local client snapshot rate, add to cl_penaltyTime",
			generate_hash("sv_local_client_snapshot_msec")
		},
		{
			"sv_maxclients",
			"The maximum number of clients that can connect to a server",
			generate_hash("sv_maxclients")
		},
		{
			"sv_minPingClamp",
			"Clamp the minimum ping to this value",
			generate_hash("sv_minPingClamp")
		},
		{
			"sv_network_fps",
			"Number of times per second the server checks for net messages",
			generate_hash("sv_network_fps")
		},
		{
			"sv_numExpBackoffBeforeReleasingCachedSnapshots",
			"if a client is under an exponential backoff over this dvar, then we will release all the cached snapshot data he owns and will send him a baseline if he reconnects",
			generate_hash("sv_numExpBackoffBeforeReleasingCachedSnapshots")
		},
		{
			"sv_paused",
			"Pause the server",
			generate_hash("sv_paused")
		},
		{
			"sv_privateClients",
			"Maximum number of private clients allowed on the server",
			generate_hash("sv_privateClients")
		},
		{
			"sv_privateClientsForClients",
			"The # of private clients (we send this to clients)",
			generate_hash("sv_privateClientsForClients")
		},
		{
			"sv_privatePassword",
			"password for the privateClient slots",
			generate_hash("sv_privatePassword")
		},
		{
			"sv_reconnectlimit",
			"minimum seconds between connect messages",
			generate_hash("sv_reconnectlimit")
		},
		{
			"sv_rejoinTimeout",
			"seconds without any message before allowing a rejoin",
			generate_hash("sv_rejoinTimeout")
		},
		{
			"sv_remote_client_snapshot_joiningstate_msec",
			"Remote client snapshot rate during join (until the client acked his first delta message)",
			generate_hash("sv_remote_client_snapshot_joiningstate_msec")
		},
		{
			"sv_remote_client_snapshot_msec",
			"Remote client snapshot rate, add to cl_penaltyTime",
			generate_hash("sv_remote_client_snapshot_msec")
		},
		{
			"sv_resetOnSpawn",
			"Have clients reset some player state fields when spawning rather than sending them over the network",
			generate_hash("sv_resetOnSpawn")
		},
		{
			"sv_running",
			"Server is running",
			generate_hash("sv_running")
		},
		{
			"sv_sayName",
			"",
			generate_hash("sv_sayName")
		},
		{
			"sv_showAverageBPS",
			"Show average bytes per second for net debugging",
			generate_hash("sv_showAverageBPS")
		},
		{
			"sv_testValue",
			"Max antilag rewind",
			generate_hash("sv_testValue")
		},
		{
			"sv_timeout",
			"seconds without any message",
			generate_hash("sv_timeout")
		},
		{
			"sv_trackFrameMsecThreshold",
			"server frame time that will trigger script time tracking.",
			generate_hash("sv_trackFrameMsecThreshold")
		},
		{
			"sv_useExtraCompress",
			"Use zlib compress for gamestate/baseline/score packets",
			generate_hash("sv_useExtraCompress")
		},
		{
			"sv_zlib_threshold",
			"Message size threshold which triggers more aggressive compression",
			generate_hash("sv_zlib_threshold")
		},
		{
			"sv_zombietime",
			"seconds to sync messages after disconnect",
			generate_hash("sv_zombietime")
		},
		{
			"svwp",
			"playerdata server write protection: 0 = disable, 1 = silent, 2 = kick",
			generate_hash("svwp")
		},
		{
			"syncTimeTimeout",
			"default timeout for sync time task (in seconds)",
			generate_hash("syncTimeTimeout")
		},
		{
			"sys_configSum",
			"Configuration checksum",
			generate_hash("sys_configSum")
		},
		{
			"sys_configureGHz",
			"Normalized total CPU power, based on cpu type, count, and speed; used in autoconfigure",
			generate_hash("sys_configureGHz")
		},
		{
			"sys_cpuGHz",
			"Measured CPU speed",
			generate_hash("sys_cpuGHz")
		},
		{
			"sys_cpuName",
			"CPU name description",
			generate_hash("sys_cpuName")
		},
		{
			"sys_gpu",
			"GPU description",
			generate_hash("sys_gpu")
		},
		{
			"sys_lockThreads",
			"Prevents specified threads from changing CPUs; improves profiling and may fix some bugs, but can hurt performance",
			generate_hash("sys_lockThreads")
		},
		{
			"sys_quitMigrateTime",
			"Time in msec to wait for host migration when user closes the window",
			generate_hash("sys_quitMigrateTime")
		},
		{
			"sys_smp_allowed",
			"Allow multi-threading",
			generate_hash("sys_smp_allowed")
		},
		{
			"sys_SSE",
			"Operating system allows Streaming SIMD Extensions",
			generate_hash("sys_SSE")
		},
		{
			"sys_sysMB",
			"Physical memory in the system",
			generate_hash("sys_sysMB")
		},
		{
			"systemlink",
			"Current game is a system link game",
			generate_hash("systemlink")
		},
		{
			"systemlink_host",
			"Local client is hosting system link game",
			generate_hash("systemlink_host")
		},
		{
			"tb_report",
			"tb event record",
			generate_hash("tb_report")
		},
		{
			"team_rebalance",
			"rebalance",
			generate_hash("team_rebalance")
		},
		{
			"teambalance_option",
			"Selects active teambalance algorithm. 0 = heuristic 1 = exhaustive",
			generate_hash("teambalance_option")
		},
		{
			"theater_active",
			"Are we allowed to show theater or not.",
			generate_hash("theater_active")
		},
		{
			"thermal_playerModel",
			"Model to draw for players when in thermal vision mode",
			generate_hash("thermal_playerModel")
		},
		{
			"thermalBlurFactorNoScope",
			"Amount of blur to use when drawing blur through a weapon's thermal scope.",
			generate_hash("thermalBlurFactorNoScope")
		},
		{
			"thermalBlurFactorScope",
			"Amount of blur to use when drawing blur through a weapon's thermal scope.",
			generate_hash("thermalBlurFactorScope")
		},
		{
			"timescale",
			"Set the game speed.",
			generate_hash("timescale")
		},
		{
			"tokensEnabled",
			"Is token economy enabled",
			generate_hash("tokensEnabled")
		},
		{
			"tracer_explosiveColor1",
			"The 1st color of a bullet tracer when using explosive bullets",
			generate_hash("tracer_explosiveColor1")
		},
		{
			"tracer_explosiveColor2",
			"The 2nd color of a bullet tracer when using explosive bullets",
			generate_hash("tracer_explosiveColor2")
		},
		{
			"tracer_explosiveColor3",
			"The 3rd color of a bullet tracer when using explosive bullets",
			generate_hash("tracer_explosiveColor3")
		},
		{
			"tracer_explosiveColor4",
			"The 4th color of a bullet tracer when using explosive bullets",
			generate_hash("tracer_explosiveColor4")
		},
		{
			"tracer_explosiveColor5",
			"The 5th color of a bullet tracer when using explosive bullets",
			generate_hash("tracer_explosiveColor5")
		},
		{
			"tracer_explosiveOverride",
			"When turned on, will apply an override to the tracer settings when shooting explosive bullets.",
			generate_hash("tracer_explosiveOverride")
		},
		{
			"tracer_explosiveWidth",
			"The width of a bullet tracer when using explosive bullets",
			generate_hash("tracer_explosiveWidth")
		},
		{
			"tracer_firstPersonMaxWidth",
			"The maximum width our OWN tracers can be when looking through our ADS",
			generate_hash("tracer_firstPersonMaxWidth")
		},
		{
			"tracer_stoppingPowerColor1",
			"The 1st color of a bullet tracer when using explosive bullets",
			generate_hash("tracer_stoppingPowerColor1")
		},
		{
			"tracer_stoppingPowerColor2",
			"The 2nd color of a bullet tracer when using explosive bullets",
			generate_hash("tracer_stoppingPowerColor2")
		},
		{
			"tracer_stoppingPowerColor3",
			"The 3rd color of a bullet tracer when using explosive bullets",
			generate_hash("tracer_stoppingPowerColor3")
		},
		{
			"tracer_stoppingPowerColor4",
			"The 4th color of a bullet tracer when using explosive bullets",
			generate_hash("tracer_stoppingPowerColor4")
		},
		{
			"tracer_stoppingPowerColor5",
			"The 5th color of a bullet tracer when using explosive bullets",
			generate_hash("tracer_stoppingPowerColor5")
		},
		{
			"tracer_stoppingPowerOverride",
			"When turned on, will apply an override to the tracer settings when shooting explosive bullets.",
			generate_hash("tracer_stoppingPowerOverride")
		},
		{
			"tracer_stoppingPowerWidth",
			"The width of a bullet tracer when using explosive bullets",
			generate_hash("tracer_stoppingPowerWidth")
		},
		{
			"tracer_thermalWidthMult",
			"The multiplier applied to the base width when viewed in thermal vision",
			generate_hash("tracer_thermalWidthMult")
		},
		{
			"transients_verbose",
			"Verbose logging information for transient fastfiles.",
			generate_hash("transients_verbose")
		},
		{
			"triggerDLCEnumerationOnSocialConfigLoad",
			"Triggers a new DLC enumeration after social config has loaded.",
			generate_hash("triggerDLCEnumerationOnSocialConfigLoad")
		},
		{
			"ui_allow_controlschange",
			"",
			generate_hash("ui_allow_controlschange")
		},
		{
			"ui_allow_teamchange",
			"",
			generate_hash("ui_allow_teamchange")
		},
		{
			"ui_autodetectGamepad",
			"undefined",
			generate_hash("ui_autodetectGamepad")
		},
		{
			"ui_autodetectGamepadDone",
			"undefined",
			generate_hash("ui_autodetectGamepadDone")
		},
		{
			"ui_bigFont",
			"Big font scale",
			generate_hash("ui_bigFont")
		},
		{
			"ui_blurAmount",
			"Max amount to blur background menu items.",
			generate_hash("ui_blurAmount")
		},
		{
			"ui_blurDarkenAmount",
			"Amount to darken blurred UI.",
			generate_hash("ui_blurDarkenAmount")
		},
		{
			"ui_blurTime",
			"Time in milliseconds to fade in/out the blur.",
			generate_hash("ui_blurTime")
		},
		{
			"ui_borderLowLightScale",
			"Scales the border color for the lowlight color on certain UI borders",
			generate_hash("ui_borderLowLightScale")
		},
		{
			"ui_browserFriendlyfire",
			"Friendly fire is active",
			generate_hash("ui_browserFriendlyfire")
		},
		{
			"ui_browserKillcam",
			"Kill cam is active",
			generate_hash("ui_browserKillcam")
		},
		{
			"ui_browserMod",
			"UI Mod value",
			generate_hash("ui_browserMod")
		},
		{
			"ui_browserShowDedicated",
			"Show dedicated servers only",
			generate_hash("ui_browserShowDedicated")
		},
		{
			"ui_browserShowEmpty",
			"Show empty servers",
			generate_hash("ui_browserShowEmpty")
		},
		{
			"ui_browserShowFull",
			"Show full servers",
			generate_hash("ui_browserShowFull")
		},
		{
			"ui_browserShowPassword",
			"Show servers that are password protected",
			generate_hash("ui_browserShowPassword")
		},
		{
			"ui_browserShowPure",
			"Show pure servers only",
			generate_hash("ui_browserShowPure")
		},
		{
			"ui_buildLocation",
			"Where to draw the build number",
			generate_hash("ui_buildLocation")
		},
		{
			"ui_buildSize",
			"Font size to use for the build number",
			generate_hash("ui_buildSize")
		},
		{
			"ui_challenge_1_ref",
			"",
			generate_hash("ui_challenge_1_ref")
		},
		{
			"ui_challenge_2_ref",
			"",
			generate_hash("ui_challenge_2_ref")
		},
		{
			"ui_challenge_3_ref",
			"",
			generate_hash("ui_challenge_3_ref")
		},
		{
			"ui_challenge_4_ref",
			"",
			generate_hash("ui_challenge_4_ref")
		},
		{
			"ui_challenge_5_ref",
			"",
			generate_hash("ui_challenge_5_ref")
		},
		{
			"ui_challenge_6_ref",
			"",
			generate_hash("ui_challenge_6_ref")
		},
		{
			"ui_challenge_7_ref",
			"",
			generate_hash("ui_challenge_7_ref")
		},
		{
			"ui_changeclass_menu_open",
			"",
			generate_hash("ui_changeclass_menu_open")
		},
		{
			"ui_changeteam_menu_open",
			"",
			generate_hash("ui_changeteam_menu_open")
		},
		{
			"ui_cinematicsTimestamp",
			"Shows cinematics timestamp on subtitle UI elements.",
			generate_hash("ui_cinematicsTimestamp")
		},
		{
			"ui_class_menu_open",
			"",
			generate_hash("ui_class_menu_open")
		},
		{
			"ui_connectScreenTextGlowColor",
			"Glow color applied to the mode and map name strings on the connect screen.",
			generate_hash("ui_connectScreenTextGlowColor")
		},
		{
			"ui_contextualMenuLocation",
			"Contextual menu location from where you entered the store.",
			generate_hash("ui_contextualMenuLocation")
		},
		{
			"ui_controls_menu_open",
			"",
			generate_hash("ui_controls_menu_open")
		},
		{
			"ui_currentFeederMapIndex",
			"Currently selected map",
			generate_hash("ui_currentFeederMapIndex")
		},
		{
			"ui_currentMap",
			"Current map index",
			generate_hash("ui_currentMap")
		},
		{
			"ui_customClassName",
			"Custom Class name",
			generate_hash("ui_customClassName")
		},
		{
			"ui_customModeEditName",
			"Name to give the currently edited custom game mode when editing is complete",
			generate_hash("ui_customModeEditName")
		},
		{
			"ui_customModeName",
			"Custom game mode name",
			generate_hash("ui_customModeName")
		},
		{
			"ui_danger_team",
			"",
			generate_hash("ui_danger_team")
		},
		{
			"ui_debugMode",
			"Draw ui debug info on the screen.",
			generate_hash("ui_debugMode")
		},
		{
			"ui_disableInGameStore",
			"This will disable the ingame store button on the xbox live menu.",
			generate_hash("ui_disableInGameStore")
		},
		{
			"ui_disableTokenRedemption",
			"This will disable the token redemption option in the in-game store menu.",
			generate_hash("ui_disableTokenRedemption")
		},
		{
			"ui_drawCrosshair",
			"Whether to draw crosshairs.",
			generate_hash("ui_drawCrosshair")
		},
		{
			"ui_editSquadMemberIndex",
			"Which squad member is currently being edited",
			generate_hash("ui_editSquadMemberIndex")
		},
		{
			"ui_extraBigFont",
			"Extra big font scale",
			generate_hash("ui_extraBigFont")
		},
		{
			"ui_game_state",
			"",
			generate_hash("ui_game_state")
		},
		{
			"ui_gametype",
			"Current game type",
			generate_hash("ui_gametype")
		},
		{
			"ui_halftime",
			"",
			generate_hash("ui_halftime")
		},
		{
			"ui_hitloc_0",
			"",
			generate_hash("ui_hitloc_0")
		},
		{
			"ui_hitloc_1",
			"",
			generate_hash("ui_hitloc_1")
		},
		{
			"ui_hitloc_2",
			"",
			generate_hash("ui_hitloc_2")
		},
		{
			"ui_hitloc_3",
			"",
			generate_hash("ui_hitloc_3")
		},
		{
			"ui_hitloc_4",
			"",
			generate_hash("ui_hitloc_4")
		},
		{
			"ui_hitloc_5",
			"",
			generate_hash("ui_hitloc_5")
		},
		{
			"ui_hitloc_damage_0",
			"",
			generate_hash("ui_hitloc_damage_0")
		},
		{
			"ui_hitloc_damage_1",
			"",
			generate_hash("ui_hitloc_damage_1")
		},
		{
			"ui_hitloc_damage_2",
			"",
			generate_hash("ui_hitloc_damage_2")
		},
		{
			"ui_hitloc_damage_3",
			"",
			generate_hash("ui_hitloc_damage_3")
		},
		{
			"ui_hitloc_damage_4",
			"",
			generate_hash("ui_hitloc_damage_4")
		},
		{
			"ui_hitloc_damage_5",
			"",
			generate_hash("ui_hitloc_damage_5")
		},
		{
			"ui_hud_hardcore",
			"Whether the HUD should be suppressed for hardcore mode",
			generate_hash("ui_hud_hardcore")
		},
		{
			"ui_hud_obituaries",
			"",
			generate_hash("ui_hud_obituaries")
		},
		{
			"ui_inactiveBaseColor",
			"The local player's rank/stats font color when shown in lobbies and parties",
			generate_hash("ui_inactiveBaseColor")
		},
		{
			"ui_inactivePartyColor",
			"",
			generate_hash("ui_inactivePartyColor")
		},
		{
			"ui_inGameStoreOpen",
			"is the InGameStore open",
			generate_hash("ui_inGameStoreOpen")
		},
		{
			"ui_inhostmigration",
			"",
			generate_hash("ui_inhostmigration")
		},
		{
			"ui_joinGametype",
			"Game join type",
			generate_hash("ui_joinGametype")
		},
		{
			"ui_loadMenuName",
			"Frontend menu will start on this level instead of lockout",
			generate_hash("ui_loadMenuName")
		},
		{
			"ui_mapname",
			"Current map name",
			generate_hash("ui_mapname")
		},
		{
			"ui_mapvote_entrya_gametype",
			"Primary map vote entry game type",
			generate_hash("ui_mapvote_entrya_gametype")
		},
		{
			"ui_mapvote_entrya_mapname",
			"Primary map vote entry map name",
			generate_hash("ui_mapvote_entrya_mapname")
		},
		{
			"ui_mapvote_entryb_gametype",
			"Secondary map vote entry game type",
			generate_hash("ui_mapvote_entryb_gametype")
		},
		{
			"ui_mapvote_entryb_mapname",
			"Secondary map vote entry map name",
			generate_hash("ui_mapvote_entryb_mapname")
		},
		{
			"ui_maxclients",
			"undefined",
			generate_hash("ui_maxclients")
		},
		{
			"ui_missingMapName",
			"Name of map to show in missing content error",
			generate_hash("ui_missingMapName")
		},
		{
			"ui_mousePitch",
			"",
			generate_hash("ui_mousePitch")
		},
		{
			"ui_multiplayer",
			"True if the game is multiplayer",
			generate_hash("ui_multiplayer")
		},
		{
			"ui_myPartyColor",
			"Player name font color when in the same party as the local player",
			generate_hash("ui_myPartyColor")
		},
		{
			"ui_netGametype",
			"Game type",
			generate_hash("ui_netGametype")
		},
		{
			"ui_netGametypeName",
			"Displayed game type name",
			generate_hash("ui_netGametypeName")
		},
		{
			"ui_netSource",
			"The network source where:\n  0:Local\n  1:Internet\n  2:Favourites",
			generate_hash("ui_netSource")
		},
		{
			"ui_onlineRequired",
			"UI requires online connection to be present.",
			generate_hash("ui_onlineRequired")
		},
		{
			"ui_opensummary",
			"",
			generate_hash("ui_opensummary")
		},
		{
			"ui_override_halftime",
			"",
			generate_hash("ui_override_halftime")
		},
		{
			"ui_partyFull",
			"True if the current party is full.",
			generate_hash("ui_partyFull")
		},
		{
			"ui_playerPartyColor",
			"",
			generate_hash("ui_playerPartyColor")
		},
		{
			"ui_playlistActionButtonAlpha",
			"The current alpha of the playlist selection button",
			generate_hash("ui_playlistActionButtonAlpha")
		},
		{
			"ui_playlistCategoryDisabledColor",
			"The color of playlist categories when disabled",
			generate_hash("ui_playlistCategoryDisabledColor")
		},
		{
			"ui_playlistCategoryEnabledColor",
			"The color of playlist categories when enabled",
			generate_hash("ui_playlistCategoryEnabledColor")
		},
		{
			"ui_promotion",
			"",
			generate_hash("ui_promotion")
		},
		{
			"ui_remoteTankUseTime",
			"",
			generate_hash("ui_remoteTankUseTime")
		},
		{
			"ui_scorelimit",
			"",
			generate_hash("ui_scorelimit")
		},
		{
			"ui_selectedFeederMap",
			"Current preview game type",
			generate_hash("ui_selectedFeederMap")
		},
		{
			"ui_serverStatusTimeOut",
			"Time in milliseconds before a server status request times out",
			generate_hash("ui_serverStatusTimeOut")
		},
		{
			"ui_show_store",
			"Use to enable the store button",
			generate_hash("ui_show_store")
		},
		{
			"ui_showDLCMaps",
			"Whether to display the DLC maps.",
			generate_hash("ui_showDLCMaps")
		},
		{
			"ui_showInfo",
			"",
			generate_hash("ui_showInfo")
		},
		{
			"ui_showList",
			"Show onscreen list of currently visible menus",
			generate_hash("ui_showList")
		},
		{
			"ui_showmap",
			"",
			generate_hash("ui_showmap")
		},
		{
			"ui_showMenuOnly",
			"If set, only menus using this name will draw.",
			generate_hash("ui_showMenuOnly")
		},
		{
			"ui_showMinimap",
			"",
			generate_hash("ui_showMinimap")
		},
		{
			"ui_sliderSteps",
			"The number of steps for a slider itemdef",
			generate_hash("ui_sliderSteps")
		},
		{
			"ui_smallFont",
			"Small font scale",
			generate_hash("ui_smallFont")
		},
		{
			"ui_textScrollFadeTime",
			"Text scrolling takes this long (seconds) to fade out at the end before restarting",
			generate_hash("ui_textScrollFadeTime")
		},
		{
			"ui_textScrollPauseEnd",
			"Text scrolling waits this long (seconds) before starting",
			generate_hash("ui_textScrollPauseEnd")
		},
		{
			"ui_textScrollPauseStart",
			"Text scrolling waits this long (seconds) before starting",
			generate_hash("ui_textScrollPauseStart")
		},
		{
			"ui_textScrollSpeed",
			"Speed at which text scrolls vertically",
			generate_hash("ui_textScrollSpeed")
		},
		{
			"ui_timelimit",
			"",
			generate_hash("ui_timelimit")
		},
		{
			"uiscript_debug",
			"spam debug info for the ui script",
			generate_hash("uiscript_debug")
		},
		{
			"unlock_breadcrumb_killswitch",
			"True to enable unlock breadcrumbs",
			generate_hash("unlock_breadcrumb_killswitch")
		},
		{
			"uno_current_tos_version",
			"Current Uno Terms of Service Version",
			generate_hash("uno_current_tos_version")
		},
		{
			"use_filtered_query_pass",
			"Dictates whether to use the filtered query for MMing or not",
			generate_hash("use_filtered_query_pass")
		},
		{
			"use_weighted_dlc_exactmatch_pass",
			"Dictates whether to use a search weighted pass with the DLC match set to exact for MMing or not",
			generate_hash("use_weighted_dlc_exactmatch_pass")
		},
		{
			"use_weighted_pass",
			"Dictates whether to use the search weighted pass for MMing or not",
			generate_hash("use_weighted_pass")
		},
		{
			"useCPMarkerForCPOwnership",
			"If set, we will check the player inventory to see if he owns the redeemedItem for a contentPack if this contentPack is not available for the player",
			generate_hash("useCPMarkerForCPOwnership")
		},
		{
			"useonlinestats",
			"Whether to use online stats when in offline modes",
			generate_hash("useonlinestats")
		},
		{
			"useRelativeTeamColors",
			"Whether to use relative team colors.",
			generate_hash("useRelativeTeamColors")
		},
		{
			"userFileFetchTimeout",
			"default timeout for user files FETCH tasks (in seconds)",
			generate_hash("userFileFetchTimeout")
		},
		{
			"userGroup_active",
			"Are we allowed to show Usergroups or not",
			generate_hash("userGroup_active")
		},
		{
			"userGroup_cool_off_time",
			"Cool off time between calls to fetch the elite clan",
			generate_hash("userGroup_cool_off_time")
		},
		{
			"userGroup_coop_delay",
			"Delay between a player joining a coop lobby and the DW user group task starting",
			generate_hash("userGroup_coop_delay")
		},
		{
			"userGroup_max_retry_time",
			"Max time that the usergroup read find can retry",
			generate_hash("userGroup_max_retry_time")
		},
		{
			"userGroup_refresh_time_secs",
			"Time in seconds between re-sending lobby group data to confirmed users.",
			generate_hash("userGroup_refresh_time_secs")
		},
		{
			"userGroup_retry_step",
			"Step in m/s for the usegroup read retry",
			generate_hash("userGroup_retry_step")
		},
		{
			"userGroup_RetryTime",
			"Time in ms between sending lobby group data retrys.",
			generate_hash("userGroup_RetryTime")
		},
		{
			"useStatsGroups",
			"If true then StatsGroups are in use for all playerdata.ddl accessing.",
			generate_hash("useStatsGroups")
		},
		{
			"useTagFlashSilenced",
			"When true, silenced weapons will use \"tag_flash_silenced\" instead of \"tag_flash\".",
			generate_hash("useTagFlashSilenced")
		},
		{
			"using_mlg",
			"MLG feature on/off",
			generate_hash("using_mlg")
		},
		{
			"validate_apply_clamps",
			"True if individual stat validation failure reverts the value",
			generate_hash("validate_apply_clamps")
		},
		{
			"validate_apply_revert",
			"True if individual stat validation failure reverts the value",
			generate_hash("validate_apply_revert")
		},
		{
			"validate_apply_revert_full",
			"True if any individual stat validation failure causes a full stats revert",
			generate_hash("validate_apply_revert_full")
		},
		{
			"validate_clamp_assists",
			"The maximum number of assists a player can make in a match",
			generate_hash("validate_clamp_assists")
		},
		{
			"validate_clamp_experience",
			"The maximum experience a player can gain in a match",
			generate_hash("validate_clamp_experience")
		},
		{
			"validate_clamp_headshots",
			"The maximum number of headshots a player can make in a match",
			generate_hash("validate_clamp_headshots")
		},
		{
			"validate_clamp_hits",
			"The maximum number of hits player can make in a match",
			generate_hash("validate_clamp_hits")
		},
		{
			"validate_clamp_kills",
			"The maximum number of kills a player can make in a match",
			generate_hash("validate_clamp_kills")
		},
		{
			"validate_clamp_losses",
			"The maximum number of losses a player can make in a match",
			generate_hash("validate_clamp_losses")
		},
		{
			"validate_clamp_misses",
			"The maximum number of misses player can make in a match",
			generate_hash("validate_clamp_misses")
		},
		{
			"validate_clamp_ties",
			"The maximum number of ties a player can make in a match",
			generate_hash("validate_clamp_ties")
		},
		{
			"validate_clamp_totalshots",
			"The maximum number of totalshots player can make in a match",
			generate_hash("validate_clamp_totalshots")
		},
		{
			"validate_clamp_weaponXP",
			"The maximum experience a weapon can gain in a match",
			generate_hash("validate_clamp_weaponXP")
		},
		{
			"validate_clamp_wins",
			"The maximum number of wins a player can make in a match",
			generate_hash("validate_clamp_wins")
		},
		{
			"validate_drop_on_fail",
			"True if stats validation failure results in dropping from the match",
			generate_hash("validate_drop_on_fail")
		},
		{
			"veh_aiOverSteerScale",
			"Scaler used to cause ai vehicles to over steer",
			generate_hash("veh_aiOverSteerScale")
		},
		{
			"veh_boneControllerLodDist",
			"Distance at which bone controllers are not updated.",
			generate_hash("veh_boneControllerLodDist")
		},
		{
			"veh_boneControllerUnLodDist",
			"Distance at which bone controllers start updating when not moving.",
			generate_hash("veh_boneControllerUnLodDist")
		},
		{
			"vehAudio_inAirPitchDownLerp",
			"Rate at which the pitch lerps down",
			generate_hash("vehAudio_inAirPitchDownLerp")
		},
		{
			"vehAudio_inAirPitchUpLerp",
			"Rate at which the pitch lerps up",
			generate_hash("vehAudio_inAirPitchUpLerp")
		},
		{
			"vehAudio_spawnVolumeTime",
			"Seconds it takes for spawned vehicles to reach full volume.",
			generate_hash("vehAudio_spawnVolumeTime")
		},
		{
			"vehCam_freeLook",
			"Enables free look mode",
			generate_hash("vehCam_freeLook")
		},
		{
			"vehCam_mode",
			"Camera modes: 1st person, 3rd person, or both",
			generate_hash("vehCam_mode")
		},
		{
			"vehDroneDebugDrawPath",
			"Debug render the drone draw paths.",
			generate_hash("vehDroneDebugDrawPath")
		},
		{
			"vehHelicopterBoundsRadius",
			"The radius of the collision volume to be used when colliding with world geometry.",
			generate_hash("vehHelicopterBoundsRadius")
		},
		{
			"vehHelicopterDecelerationFwd",
			"Set the deceleration of the player helicopter (as a fraction of acceleration) in the direction the chopper is facing.  So 1.0 makes it equal to the acceleration.",
			generate_hash("vehHelicopterDecelerationFwd")
		},
		{
			"vehHelicopterDecelerationSide",
			"Set the side-to-side deceleration of the player helicopter (as a fraction of acceleration).  So 1.0 makes it equal to the acceleration.",
			generate_hash("vehHelicopterDecelerationSide")
		},
		{
			"vehHelicopterDecelerationUp",
			"Set the vertical deceleration of the player helicopter (as a fraction of acceleration).  So 1.0 makes it equal to the acceleration.",
			generate_hash("vehHelicopterDecelerationUp")
		},
		{
			"vehHelicopterHeadSwayDontSwayTheTurret",
			"If set, the turret will not fire through the crosshairs, but straight ahead of the vehicle, when the player is not freelooking.",
			generate_hash("vehHelicopterHeadSwayDontSwayTheTurret")
		},
		{
			"vehHelicopterHoverSpeedThreshold",
			"The speed below which the player helicopter begins to jitter the tilt, for hovering",
			generate_hash("vehHelicopterHoverSpeedThreshold")
		},
		{
			"vehHelicopterInvertUpDown",
			"Invert the altitude control on the player helicopter.",
			generate_hash("vehHelicopterInvertUpDown")
		},
		{
			"vehHelicopterJitterJerkyness",
			"Specifies how jerky the tilt jitter should be",
			generate_hash("vehHelicopterJitterJerkyness")
		},
		{
			"vehHelicopterLookaheadTime",
			"How far ahead (in seconds) the player helicopter looks ahead, to avoid hard collisions.  (Like driving down the highway, you should keep 2 seconds distance between you and the vehicle in front of you)",
			generate_hash("vehHelicopterLookaheadTime")
		},
		{
			"vehHelicopterMaxAccel",
			"Maximum horizontal acceleration of the player helicopter (in MPH per second)",
			generate_hash("vehHelicopterMaxAccel")
		},
		{
			"vehHelicopterMaxAccelVertical",
			"Maximum vertical acceleration of the player helicopter (in MPH per second)",
			generate_hash("vehHelicopterMaxAccelVertical")
		},
		{
			"vehHelicopterMaxPitch",
			"Maximum pitch of the player helicopter",
			generate_hash("vehHelicopterMaxPitch")
		},
		{
			"vehHelicopterMaxRoll",
			"Maximum roll of the player helicopter",
			generate_hash("vehHelicopterMaxRoll")
		},
		{
			"vehHelicopterMaxSpeed",
			"Maximum horizontal speed of the player helicopter (in MPH)",
			generate_hash("vehHelicopterMaxSpeed")
		},
		{
			"vehHelicopterMaxSpeedVertical",
			"Maximum vertical speed of the player helicopter (in MPH)",
			generate_hash("vehHelicopterMaxSpeedVertical")
		},
		{
			"vehHelicopterMaxYawAccel",
			"Maximum yaw acceleration of the player helicopter",
			generate_hash("vehHelicopterMaxYawAccel")
		},
		{
			"vehHelicopterMaxYawRate",
			"Maximum yaw speed of the player helicopter",
			generate_hash("vehHelicopterMaxYawRate")
		},
		{
			"vehHelicopterPitchOffset",
			"The resting pitch of the helicopter",
			generate_hash("vehHelicopterPitchOffset")
		},
		{
			"vehHelicopterRightStickDeadzone",
			"Dead-zone for the axes of the right thumbstick.  This helps to better control the two axes separately.",
			generate_hash("vehHelicopterRightStickDeadzone")
		},
		{
			"vehHelicopterScaleMovement",
			"Scales down the smaller of the left stick axes.",
			generate_hash("vehHelicopterScaleMovement")
		},
		{
			"vehHelicopterSoftCollisions",
			"Player helicopters have soft collisions (slow down before they collide).",
			generate_hash("vehHelicopterSoftCollisions")
		},
		{
			"vehHelicopterStrafeDeadzone",
			"Dead-zone so that you can fly straight forward easily without accidentally strafing (and thus rolling).",
			generate_hash("vehHelicopterStrafeDeadzone")
		},
		{
			"vehHelicopterTiltFromAcceleration",
			"The amount of tilt caused by acceleration",
			generate_hash("vehHelicopterTiltFromAcceleration")
		},
		{
			"vehHelicopterTiltFromControllerAxes",
			"The amount of tilt caused by the desired velocity (i.e., the amount of controller stick deflection)",
			generate_hash("vehHelicopterTiltFromControllerAxes")
		},
		{
			"vehHelicopterTiltFromDeceleration",
			"The amount of tilt caused by deceleration",
			generate_hash("vehHelicopterTiltFromDeceleration")
		},
		{
			"vehHelicopterTiltFromFwdAndYaw",
			"The amount of roll caused by yawing while moving forward.",
			generate_hash("vehHelicopterTiltFromFwdAndYaw")
		},
		{
			"vehHelicopterTiltFromFwdAndYaw_VelAtMaxTilt",
			"The forward speed (as a fraction of top speed) at which the tilt due to yaw reaches is maximum value.",
			generate_hash("vehHelicopterTiltFromFwdAndYaw_VelAtMaxTilt")
		},
		{
			"vehHelicopterTiltFromVelocity",
			"The amount of tilt caused by the current velocity",
			generate_hash("vehHelicopterTiltFromVelocity")
		},
		{
			"vehHelicopterTiltMomentum",
			"The amount of rotational momentum the helicopter has with regards to tilting.",
			generate_hash("vehHelicopterTiltMomentum")
		},
		{
			"vehHelicopterTiltSpeed",
			"The rate at which the player helicopter's tilt responds",
			generate_hash("vehHelicopterTiltSpeed")
		},
		{
			"vehHelicopterYawOnLeftStick",
			"The yaw speed created by the left stick when pushing the stick diagonally (e.g., moving forward and strafing slightly).",
			generate_hash("vehHelicopterYawOnLeftStick")
		},
		{
			"vehicle_debug_render_spline_plane",
			"Do we want to render the spline plane data",
			generate_hash("vehicle_debug_render_spline_plane")
		},
		{
			"vehicle_pathsmooth",
			"Smoothed vehicle pathing.",
			generate_hash("vehicle_pathsmooth")
		},
		{
			"vehUGVPitchTrack",
			"UGV body pitch orientation speed",
			generate_hash("vehUGVPitchTrack")
		},
		{
			"vehUGVRollTrack",
			"UGV body roll orientation speed",
			generate_hash("vehUGVRollTrack")
		},
		{
			"vehUGVWheelInfluence",
			"UGV wheel influence on the orientation of the body",
			generate_hash("vehUGVWheelInfluence")
		},
		{
			"vehWalkerControlMode",
			"Walker controls (0==move no turn, 1=move and turn, 2=move relative(tank))",
			generate_hash("vehWalkerControlMode")
		},
		{
			"version",
			"Game version",
			generate_hash("version")
		},
		{
			"vid_xpos",
			"Game window horizontal position",
			generate_hash("vid_xpos")
		},
		{
			"vid_ypos",
			"Game window vertical position",
			generate_hash("vid_ypos")
		},
		{
			"viewangNow",
			"",
			generate_hash("viewangNow")
		},
		{
			"viewModelDebugNotetracks",
			"Enable display of viewmodel notetrack debug info.",
			generate_hash("viewModelDebugNotetracks")
		},
		{
			"viewModelHacks",
			"Enabled depth hack and remove viewmodel from shadows.",
			generate_hash("viewModelHacks")
		},
		{
			"viewposNow",
			"",
			generate_hash("viewposNow")
		},
		{
			"virtualLobbyActive",
			"Indicates the VL is actively being displayed.",
			generate_hash("virtualLobbyActive")
		},
		{
			"virtualLobbyAllocated",
			"Indicates the first VL zone has been loaded.",
			generate_hash("virtualLobbyAllocated")
		},
		{
			"virtualLobbyEnabled",
			"VirtualLobby is enabled (must be true before loading UI zone)",
			generate_hash("virtualLobbyEnabled")
		},
		{
			"virtualLobbyInFiringRange",
			"VirtualLobby is in firing range mode",
			generate_hash("virtualLobbyInFiringRange")
		},
		{
			"virtualLobbyMap",
			"VirtualLobby map to load (must be set before starting vl)",
			generate_hash("virtualLobbyMap")
		},
		{
			"virtualLobbyMembers",
			"Number of members in the VirtualLobby (set by script)",
			generate_hash("virtualLobbyMembers")
		},
		{
			"virtualLobbyPresentable",
			"Indicates to LUA the VirtualLobby is ready to be displayed (set by script).",
			generate_hash("virtualLobbyPresentable")
		},
		{
			"virtualLobbyReady",
			"Indicates to LUA the VirtualLobby is loaded and running (set by script).",
			generate_hash("virtualLobbyReady")
		},
		{
			"vl_clan_models_loaded",
			"Indicates to LUA when all models are loaded for the clan highlights so it can begin the fade-in without any popping(set by script).",
			generate_hash("vl_clan_models_loaded")
		},
		{
			"voMtxEnable",
			"When set (e.g. via config), will enable voice over packs",
			generate_hash("voMtxEnable")
		},
		{
			"waypointAerialIconMaxSize",
			"Max size of aerial targeting waypoints.",
			generate_hash("waypointAerialIconMaxSize")
		},
		{
			"waypointAerialIconMinSize",
			"Min size of aerial targeting waypoints.",
			generate_hash("waypointAerialIconMinSize")
		},
		{
			"waypointAerialIconScale",
			"Base scale of aerial targeting waypoints.",
			generate_hash("waypointAerialIconScale")
		},
		{
			"waypointDebugDraw",
			"",
			generate_hash("waypointDebugDraw")
		},
		{
			"waypointDistScaleRangeMax",
			"Distance from player that icon distance scaling ends.",
			generate_hash("waypointDistScaleRangeMax")
		},
		{
			"waypointDistScaleRangeMin",
			"Distance from player that icon distance scaling ends.",
			generate_hash("waypointDistScaleRangeMin")
		},
		{
			"waypointDistScaleSmallest",
			"Smallest scale that the distance effect uses.",
			generate_hash("waypointDistScaleSmallest")
		},
		{
			"waypointIconHeight",
			"",
			generate_hash("waypointIconHeight")
		},
		{
			"waypointIconWidth",
			"",
			generate_hash("waypointIconWidth")
		},
		{
			"waypointOffscreenCornerRadius",
			"Size of the rounded corners.",
			generate_hash("waypointOffscreenCornerRadius")
		},
		{
			"waypointOffscreenDistanceThresholdAlpha",
			"Distance from the threshold over which offscreen objective icons lerp their alpha.",
			generate_hash("waypointOffscreenDistanceThresholdAlpha")
		},
		{
			"waypointOffscreenPadBottom",
			"",
			generate_hash("waypointOffscreenPadBottom")
		},
		{
			"waypointOffscreenPadLeft",
			"",
			generate_hash("waypointOffscreenPadLeft")
		},
		{
			"waypointOffscreenPadRight",
			"",
			generate_hash("waypointOffscreenPadRight")
		},
		{
			"waypointOffscreenPadTop",
			"",
			generate_hash("waypointOffscreenPadTop")
		},
		{
			"waypointOffscreenPointerDistance",
			"Distance from the center of the offscreen objective icon to the center its arrow.",
			generate_hash("waypointOffscreenPointerDistance")
		},
		{
			"waypointOffscreenPointerHeight",
			"",
			generate_hash("waypointOffscreenPointerHeight")
		},
		{
			"waypointOffscreenPointerWidth",
			"",
			generate_hash("waypointOffscreenPointerWidth")
		},
		{
			"waypointOffscreenRoundedCorners",
			"Off-screen icons take rounded corners when true.  90-degree corners when false.",
			generate_hash("waypointOffscreenRoundedCorners")
		},
		{
			"waypointOffscreenScaleLength",
			"How far the offscreen icon scale travels from full to smallest scale.",
			generate_hash("waypointOffscreenScaleLength")
		},
		{
			"waypointOffscreenScaleSmallest",
			"Smallest scale that the offscreen effect uses.",
			generate_hash("waypointOffscreenScaleSmallest")
		},
		{
			"waypointPlayerOffsetCrouch",
			"For waypoints pointing to players, how high to offset off of their origin when they are prone.",
			generate_hash("waypointPlayerOffsetCrouch")
		},
		{
			"waypointPlayerOffsetProne",
			"For waypoints pointing to players, how high to offset off of their origin when they are prone.",
			generate_hash("waypointPlayerOffsetProne")
		},
		{
			"waypointPlayerOffsetStand",
			"For waypoints pointing to players, how high to offset off of their origin when they are prone.",
			generate_hash("waypointPlayerOffsetStand")
		},
		{
			"waypointScreenCenterFadeAdsMin",
			"When 'waypointScreenCenterFadeRadius' enabled, minimum amount that waypoint will fade when in ads",
			generate_hash("waypointScreenCenterFadeAdsMin")
		},
		{
			"waypointScreenCenterFadeHipMin",
			"When 'waypointScreenCenterFadeRadius' enabled, minimum amount that waypoint will fade when in ads",
			generate_hash("waypointScreenCenterFadeHipMin")
		},
		{
			"waypointScreenCenterFadeRadius",
			"Radius from screen center that a waypoint will start fading out. Setting to 0 will turn this off",
			generate_hash("waypointScreenCenterFadeRadius")
		},
		{
			"waypointSplitscreenScale",
			"Scale applied to waypoint icons in splitscreen views.",
			generate_hash("waypointSplitscreenScale")
		},
		{
			"waypointTweakY",
			"",
			generate_hash("waypointTweakY")
		},
		{
			"weap_thermoDebuffMod",
			"",
			generate_hash("weap_thermoDebuffMod")
		},
		{
			"wideScreen",
			"True if the game video is running in 16x9 aspect, false if 4x3.",
			generate_hash("wideScreen")
		},
		{
			"winvoice_loopback",
			"Echo microphone input locally",
			generate_hash("winvoice_loopback")
		},
		{
			"winvoice_mic_mute",
			"Mute the microphone",
			generate_hash("winvoice_mic_mute")
		},
		{
			"winvoice_mic_outTime",
			"Microphone voice amount of silence before we cut the mic",
			generate_hash("winvoice_mic_outTime")
		},
		{
			"winvoice_mic_reclevel",
			"Microphone recording level",
			generate_hash("winvoice_mic_reclevel")
		},
		{
			"winvoice_mic_scaler",
			"Microphone scaler value",
			generate_hash("winvoice_mic_scaler")
		},
		{
			"winvoice_mic_threshold",
			"Microphone voice threshold",
			generate_hash("winvoice_mic_threshold")
		},
		{
			"winvoice_save_voice",
			"Write voice data to a file",
			generate_hash("winvoice_save_voice")
		},
		{
			"xanim_disableIK",
			"Disable inverse kinematics solvers",
			generate_hash("xanim_disableIK")
		},
		{
			"xblive_competitionmatch",
			"MLG Rules?",
			generate_hash("xblive_competitionmatch")
		},
		{
			"xblive_hostingprivateparty",
			"true only if we're hosting a party",
			generate_hash("xblive_hostingprivateparty")
		},
		{
			"xblive_loggedin",
			"User is logged into xbox live",
			generate_hash("xblive_loggedin")
		},
		{
			"xblive_privatematch",
			"Current game is a private match",
			generate_hash("xblive_privatematch")
		},
		{
			"xblive_privatematch_solo",
			"Current game is an Extinction solo match",
			generate_hash("xblive_privatematch_solo")
		},
		{
			"xphys_maxJointPositionError",
			"If a joints with position error exceeding this value is detected, then the whole xphys system gets snapped back to the animation pose",
			generate_hash("xphys_maxJointPositionError")
		},
		{
			"aa_player_kills",
			"Player kills",
			generate_hash("aa_player_kills")
		},
		{
			"aa_player_damage_dealt",
			"Player damages dealt",
			generate_hash("aa_player_damage_dealt")
		},
		{
			"aa_ads_damage_dealt",
			"Player damages dealt in ads",
			generate_hash("aa_ads_damage_dealt")
		},
		{
			"aa_deaths",
			"Player deaths",
			generate_hash("aa_deaths")
		},
		{
			"aa_time_tracking",
			"Time in game",
			generate_hash("aa_time_tracking")
		},
	};

	std::string dvar_get_description(const std::string& name)
	{
		const auto lower = utils::string::to_lower(name);
		for (std::uint32_t i = 0; i < dvar_list.size(); i++)
		{
			if (utils::string::to_lower(dvar_list[i].name) == lower)
			{
				return dvar_list[i].description;
			}
		}

		return {};
	}

	bool can_add_dvar_to_list(std::string name)
	{
		const auto lower = utils::string::to_lower(name);
		for (std::uint32_t i = 0; i < dvar_list.size(); i++)
		{
			if (utils::string::to_lower(dvar_list[i].name) == lower)
			{
				return false;
			}
		}

		return true;
	}

	std::optional<dvar_info> get_dvar_info_from_hash(const int hash)
	{
		for (std::uint32_t i = 0; i < dvar_list.size(); i++)
		{
			if (dvar_list[i].hash == hash)
			{
				return {dvar_list[i]};
			}
		}

		return {};
	}

	std::string hash_to_string(const int hash)
	{
		return utils::string::va("0x%lX", hash);
	}

	game::dvar_t* register_int(const std::string& name, int value, int min, int max,
		game::DvarFlags flags, const std::string& description)
	{
		const auto hash = game::generateHashValue(name.data());

		if (can_add_dvar_to_list(name))
		{
			dvar_list.push_back({name, description});
		}

		return game::Dvar_RegisterInt(hash, "", value, min, max, flags);
	}

	game::dvar_t* register_bool(const std::string& name, bool value,
		game::DvarFlags flags, const std::string& description)
	{
		const auto hash = game::generateHashValue(name.data());

		if (can_add_dvar_to_list(name))
		{
			dvar_list.push_back({name, description});
		}

		return game::Dvar_RegisterBool(hash, "", value, flags);
	}

	game::dvar_t* register_string(const std::string& name, const char* value,
		game::DvarFlags flags, const std::string& description)
	{
		const auto hash = game::generateHashValue(name.data());

		if (can_add_dvar_to_list(name))
		{
			dvar_list.push_back({name, description});
		}

		return game::Dvar_RegisterString(hash, "", value, flags);
	}

	game::dvar_t* register_float(const std::string& name, float value, float min,
		float max, game::DvarFlags flags, const std::string& description)
	{
		const auto hash = game::generateHashValue(name.data());

		if (can_add_dvar_to_list(name))
		{
			dvar_list.push_back({name, description});
		}

		return game::Dvar_RegisterFloat(hash, "", value, min, max, flags);
	}

	game::dvar_t* register_vec4(const std::string& name, float x, float y, float z,
		float w, float min, float max, game::DvarFlags flags, const std::string& description)
	{
		const auto hash = game::generateHashValue(name.data());

		if (can_add_dvar_to_list(name))
		{
			dvar_list.push_back({name, description});
		}

		return game::Dvar_RegisterVec4(hash, "", x, y, z, w, min, max, flags);
	}
}
