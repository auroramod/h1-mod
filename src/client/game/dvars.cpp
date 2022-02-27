#include <std_include.hpp>
#include <utils/string.hpp>

#include "game.hpp"
#include <component/console.hpp>

namespace dvars
{
	game::dvar_t* con_inputBoxColor = nullptr;
	game::dvar_t* con_inputHintBoxColor = nullptr;
	game::dvar_t* con_outputBarColor = nullptr;
	game::dvar_t* con_outputSliderColor = nullptr;
	game::dvar_t* con_outputWindowColor = nullptr;
	game::dvar_t* con_inputDvarMatchColor = nullptr;
	game::dvar_t* con_inputDvarValueColor = nullptr;
	game::dvar_t* con_inputDvarInactiveValueColor = nullptr;
	game::dvar_t* con_inputCmdMatchColor = nullptr;

	game::dvar_t* jump_enableFallDamage;

	game::dvar_t* r_fullbright;
	game::dvar_t* r_chams;

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

	std::vector<std::string> dvar_list =
	{
		"accessToSubscriberContent",
		"acousticSpikeMaxRange",
		"acousticSpikeMinRadius",
		"acousticSpikeMinRange",
		"acousticSpikeRingSize",
		"acousticSpikeSize",
		"actionSlotsHide",
		"activeAction",
		"activeFriendsMaxBackoffLevel",
		"activeFriendsNumDayBuckets",
		"activeFriendsNumPlayBuckets",
		"activeFriendsRefreshDelay",
		"activeFriendsSecondsPerBucket",
		"aim_accel_turnrate_lerp",
		"aim_aimAssistRangeScale",
		"aim_alternate_lockon_deflection",
		"aim_alternate_lockon_pitch_strength",
		"aim_alternate_lockon_region_height",
		"aim_alternate_lockon_region_width",
		"aim_alternate_lockon_strength",
		"aim_assist_min_target_distance",
		"aim_assist_script_disable",
		"bot_AllowGrenades",
		"bot_autoconnectdefault",
		"bot_CloseDistance",
		"bot_CrouchDistance",
		"bot_difficulty",
		"bot_difficultydefault",
		"bot_enemies",
		"bot_Fov",
		"bot_friends",
		"bot_GoalRadius",
		"bot_MaxAdsTime",
		"bot_MaxCrouchTime",
		"bot_MaxDeathTime",
		"bot_MaxFireTime",
		"bot_MaxGrenadeTime",
		"bot_MaxPitchTime",
		"bot_MaxReactionTime",
		"bot_MaxStrafeTime",
		"bot_MeleeDist",
		"bot_MinAdsTime",
		"bot_MinCrouchTime",
		"bot_MinDeathTime",
		"bot_MinFireTime",
		"bot_MinGrenadeTime",
		"bot_MinPitchTime",
		"bot_MinReactionTime",
		"bot_MinStrafeTime",
		"bot_PitchDown",
		"bot_PitchSpeed",
		"bot_PitchSpeedAds",
		"bot_PitchUp",
		"bot_SprintDistance",
		"bot_StrafeChance",
		"bot_TargetLeadBias",
		"bot_tips",
		"bot_UseFriendNames",
		"bot_YawSpeed",
		"bot_YawSpeedAds",
		"cg_draw2D",
		"cg_drawBigFPS",
		"cg_drawBreathHint",
		"cg_drawBuildName",
		"cg_drawFPS",
		"cg_drawFPSLabels",
		"cg_drawFPSOnly",
		"cg_drawFPSScale",
		"cg_drawgun",
		"cg_drawPing",
		"cg_drawVersion",
		"cg_drawVersionX",
		"cg_drawVersionY",
		"cg_drawViewpos",
		"cg_fov",
		"cg_fov_default",
		"cg_fov_default_thirdperson",
		"cg_fovCompMax",
		"cg_fovExtraCam",
		"cg_fovMin",
		"cg_fovScale",
		"cg_legacyCrashHandling",
		"cl_connectTimeout",
		"cl_maxpackets",
		"cl_maxPing",
		"cl_timeout",
		"codPointStore_enabled",
		"com_introPlayed",
		"com_isNotice",
		"com_maxclients",
		"com_maxfps",
		"com_maxFrameTime",
		"custom_roundlimit",
		"custom_scorelimit",
		"custom_scr_allowannouncer",
		"custom_scr_allowbattlechatter",
		"custom_scr_bot_difficulty",
		"custom_scr_ctf_enemycarriervisible",
		"custom_scr_ctf_idleflagreturntime",
		"custom_scr_ctf_roundswitch",
		"custom_scr_ctf_touchreturn",
		"custom_scr_custom_score_assist",
		"custom_scr_dem_bombtimer",
		"custom_scr_dem_defusetime",
		"custom_scr_dem_extratime",
		"custom_scr_dem_planttime",
		"custom_scr_dem_roundswitch",
		"custom_scr_dm_bonus_leader",
		"custom_scr_dm_score_assist",
		"custom_scr_dm_score_death",
		"custom_scr_dm_score_headshot",
		"custom_scr_dm_score_kill",
		"custom_scr_dm_score_suicide",
		"custom_scr_dom_flagcapturetime",
		"custom_scr_game_allowkillcam",
		"custom_scr_game_forceradar",
		"custom_scr_game_hardpoints",
		"custom_scr_game_onlyheadshots",
		"custom_scr_game_perks",
		"custom_scr_game_spectatetype",
		"custom_scr_hardcore",
		"custom_scr_num_bots",
		"custom_scr_num_bots_enemy",
		"custom_scr_num_bots_friendly",
		"custom_scr_player_forcerespawn",
		"custom_scr_player_healthregentime",
		"custom_scr_player_maxhealth",
		"custom_scr_player_sprintTime",
		"custom_scr_rcbomb_notimeout",
		"custom_scr_sab_bombtimer",
		"custom_scr_sab_defusetime",
		"custom_scr_sab_hotpotato",
		"custom_scr_sab_planttime",
		"custom_scr_sab_roundswitch",
		"custom_scr_sd_bombtimer",
		"custom_scr_sd_defusetime",
		"custom_scr_sd_multibomb",
		"custom_scr_sd_planttime",
		"custom_scr_sd_roundswitch",
		"custom_scr_tdm_bonus_leader",
		"custom_scr_tdm_score_death",
		"custom_scr_tdm_score_headshot",
		"custom_scr_tdm_score_kill",
		"custom_scr_tdm_score_suicide",
		"custom_scr_team_fftype",
		"custom_scr_team_teamkillspawndelay",
		"custom_scr_vehicles_enabled",
		"custom_timelimit",
		"elite_clan_active",
		"fs_basegame",
		"fs_basepath",
		"fs_basepath_output",
		"fs_cdpath",
		"fs_copyfiles",
		"fs_debug",
		"fs_game",
		"fs_homepath",
		"fs_ignoreLocalized",
		"fs_restrict",
		"fs_savepath",
		"fs_usedevdir",
		"fs_userDocuments",
		"fs_usermapdir",
		"g_gametype",
		"g_hardcore",
		"g_listEntity",
		"g_loadScripts",
		"g_log",
		"g_logSync",
		"g_logTimeStampInSeconds",
		"g_motd",
		"g_scriptMainMenu",
		"g_smoothClients",
		"g_spawnai",
		"g_speed",
		"gamedate",
		"gamedvr_active",
		"gameMode",
		"gamename",
		"gametype",
		"log_party_state",
		"logfile",
		"m_filter",
		"m_forward",
		"m_pitch",
		"m_side",
		"m_yaw",
		"map",
		"mapname",
		"r_drawLightmapDrawlists",
		"r_drawLitDrawlists",
		"r_drawSun",
		"r_drawWater",
		"r_fog",
		"r_fog_depthhack_scale",
		"r_fog_disable",
		"r_fog_ev_adjust",
		"r_fogBaseDist",
		"r_fogBaseHeight",
		"r_fogColor",
		"r_fogHalfDist",
		"r_fogHalfHeight",
		"r_fogOpacity",
		"r_fogSunColor",
		"r_fogSunInner",
		"r_fogSunOpacity",
		"r_fogSunOuter",
		"r_fogSunPitch",
		"r_fogSunYaw",
		"r_fogTweak",
		"r_forceLod",
		"r_fullbright",
		"r_fullPrepass",
		"r_fullscreen",
		"r_fullscreenWindow",
		"r_fxaa",
		"r_fxaaSubpixel",
		"r_FXAverageColorFunc",
		"r_gamma",
		"r_glossMap",
		"r_glow",
		"r_glow_allowed",
		"safeArea_adjusted_horizontal",
		"safeArea_adjusted_vertical",
		"safeArea_horizontal",
		"safeArea_vertical",
		"scr_game_spectatetype",
		"sensitivity",
		"stat_version",
		"stats_version_check",
		"statsLocationFatal",
		"stopspeed",
		"storeMapPackMaskToStats",
		"stringtable_debug",
		"sv_allowAnonymous",
		"sv_allowClientConsole",
		"sv_allowDownload",
		"sv_allowedClan1",
		"sv_allowedClan2",
		"sv_archiveClientsPositions",
		"sv_assistWorkers",
		"sv_authenticating",
		"sv_autoPriority",
		"sv_bitfieldTracking",
		"sv_botsPressAttackBtn",
		"sv_cheats",
		"sv_checkMinPlayers",
		"sv_clientArchive",
		"sv_clientFpsLimit",
		"sv_clientside",
		"sv_clientSideBullets",
		"sv_clientSideVehicles",
		"sv_connectTimeout",
		"sv_cumulThinkTime",
		"sv_debugRate",
		"sv_debugReliableCmds",
		"sv_disableClientConsole",
		"sv_dwlsgerror",
		"sv_endGameIfISuck",
		"sv_error_on_baseline_failure",
		"sv_expensive_bullet_time",
		"sv_exponentialBackoffAfterNonAckedMsgs",
		"sv_externalEventLoop",
		"sv_FakeRemoteClient",
		"sv_fakeServerLoad",
		"sv_fakeServerLoadRand",
		"sv_FFCheckSums",
		"sv_FFNames",
		"sv_floodprotect",
		"sv_forceunranked",
		"sv_fps",
		"sv_hostname",
		"sv_hugeSnapshotDelay",
		"sv_hugeSnapshotSize",
		"sv_iwdNames",
		"sv_iwds",
		"sv_keywords",
		"sv_kickBanTime",
		"sv_lastSaveCommitedToDevice",
		"sv_local_client_snapshot_msec",
		"sv_mapname",
		"sv_mapRotation",
		"sv_mapRotationCurrent",
		"sv_maxclients",
		"sv_maxPhysExplosionSpheres",
		"sv_maxPing",
		"sv_maxRate",
		"sv_minPing",
		"sv_minPingClamp",
		"sv_network_fps",
		"sv_networkRateSolution",
		"sv_noname",
		"sv_NoShapshotWarnings",
		"sv_numExpBackoffBeforeReleasingCachedSnapshots",
		"sv_packet_info",
		"sv_padPackets",
		"sv_paused",
		"sv_playlistFetchInterval",
		"sv_privateClients",
		"sv_privateClientsForClients",
		"sv_privatePassword",
		"sv_punkbuster",
		"sv_pure",
		"sv_reconnectlimit",
		"sv_referencedFFCheckSums",
		"sv_referencedFFNames",
		"sv_referencedIwdNames",
		"sv_referencedIwds",
		"sv_rejoinTimeout",
		"sv_remote_client_snapshot_joiningstate_msec",
		"sv_remote_client_snapshot_msec",
		"sv_resetOnSpawn",
		"sv_restrictedTempEnts",
		"sv_rewindPoseArchive",
		"sv_running",
		"sv_saveDeviceAvailable",
		"sv_saveGameAvailable",
		"sv_saveGameNotReadable",
		"sv_saveOnStartMap",
		"sv_serverid",
		"sv_showAverageBPS",
		"sv_showCommands",
		"sv_smp",
		"sv_SnapshotManLaw",
		"sv_testValue",
		"sv_timeout",
		"sv_trackFrameMsecThreshold",
		"sv_useExtraCompress",
		"sv_voice",
		"sv_voiceQuality",
		"sv_writeConfigStrings",
		"sv_wwwBaseURL",
		"sv_wwwDlDisconnected",
		"sv_wwwDownload",
		"sv_zlib_threshold",
		"sv_zombietime",
		"svwp",
		"syncTimeTimeout",
		"sys_configSum",
		"sys_configureGHz",
		"sys_cpuGHz",
		"sys_cpuName",
		"sys_gpu",
		"sys_lockThreads",
		"sys_quitMigrateTime",
		"sys_smp_allowed",
		"sys_SSE",
		"sys_sysMB",
		"systemlink",
		"systemlink_host",
		"timescale", // Scale time of each frame		  ---> "5401"
		"ui_mousePitch",
		//"xpartygo"
	};

	bool can_add_dvar_to_list(std::string name)
	{
		for (std::uint32_t i = 0; i < dvar_list.size(); i++)
		{
			if (dvar_list[i] == name)
			{
				return false;
			}
		}
		return true;
	}

	game::dvar_t* register_int(const std::string& name, int value, int min, int max,
		game::DvarFlags flags, bool add_to_list)
	{
		const auto hash = game::generateHashValue(name.data());

		if (add_to_list && can_add_dvar_to_list(name))
		{
			dvar_list.push_back(name);
		}

		return game::Dvar_RegisterInt(hash, "", value, min, max, flags);
	}

	game::dvar_t* register_bool(const std::string& name, bool value,
		game::DvarFlags flags, bool add_to_list)
	{
		const auto hash = game::generateHashValue(name.data());

		if (add_to_list && can_add_dvar_to_list(name))
		{
			dvar_list.push_back(name);
		}

		return game::Dvar_RegisterBool(hash, "", value, flags);
	}

	game::dvar_t* register_string(const std::string& name, const char* value,
		game::DvarFlags flags, bool add_to_list)
	{
		const auto hash = game::generateHashValue(name.data());

		if (add_to_list && can_add_dvar_to_list(name))
		{
			dvar_list.push_back(name);
		}

		return game::Dvar_RegisterString(hash, "", value, flags);
	}

	game::dvar_t* register_float(const std::string& name, float value, float min,
		float max, game::DvarFlags flags, bool add_to_list)
	{
		const auto hash = game::generateHashValue(name.data());

		if (add_to_list && can_add_dvar_to_list(name))
		{
			dvar_list.push_back(name);
		}

		return game::Dvar_RegisterFloat(hash, "", value, min, max, flags);
	}

	game::dvar_t* register_vec4(const std::string& name, float x, float y, float z,
		float w, float min, float max, game::DvarFlags flags, bool add_to_list)
	{
		const auto hash = game::generateHashValue(name.data());

		if (add_to_list && can_add_dvar_to_list(name))
		{
			dvar_list.push_back(name);
		}

		return game::Dvar_RegisterVec4(hash, "", x, y, z, w, min, max, flags);
	}

	namespace override 
	{
		static std::unordered_map<std::string, std::string> set_string_overrides;

		game::dvar_t* register_int(const std::string& name, int value, int min, int max,
			const unsigned int flags, bool add_to_list)
		{
			const auto hash = game::generateHashValue(name.data());

			if (add_to_list && can_add_dvar_to_list(name))
			{
				dvar_list.push_back(name);
			}

			return game::Dvar_RegisterInt(hash, "", value, min, max, flags);
		}

		void Dvar_SetString(const std::string& name, const std::string& value)
		{
			set_string_overrides[name] = value;
		}

		game::dvar_t* register_float(const std::string& name, float value, float min,
			float max, game::DvarFlags flags, bool add_to_list)
		{
			const auto hash = game::generateHashValue(name.data());

			if (add_to_list && can_add_dvar_to_list(name))
			{
				dvar_list.push_back(name);
			}

			return game::Dvar_RegisterFloat(hash, "", value, min, max, flags);
		}

		game::dvar_t* register_string(const std::string& name, const char* value,
			game::DvarFlags flags, bool add_to_list)
		{
			const auto hash = game::generateHashValue(name.data());

			if (add_to_list && can_add_dvar_to_list(name))
			{
				dvar_list.push_back(name);
			}

			return game::Dvar_RegisterString(hash, "", value, flags);
		}
	}
}
