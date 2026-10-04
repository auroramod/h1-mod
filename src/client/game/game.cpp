#include <std_include.hpp>
#include "game.hpp"

namespace game
{
	uint64_t base_address = 0x140000000;

	int Cmd_Argc()
	{
		return cmd_args->argc[cmd_args->nesting];
	}

	const char* Cmd_Argv(const int index)
	{
		return cmd_args->argv[cmd_args->nesting][index];
	}

	int SV_Cmd_Argc()
	{
		return sv_cmd_args->argc[sv_cmd_args->nesting];
	}

	const char* SV_Cmd_Argv(const int index)
	{
		return sv_cmd_args->argv[sv_cmd_args->nesting][index];
	}

	bool VirtualLobby_Loaded()
	{
		return !game::environment::is_sp() && *mp::virtualLobby_loaded == 1;
	}

	void SV_GameSendServerCommand(int client_num, svscmd_type type, const char* text)
	{
		if (client_num == -1)
		{
			SV_SendServerCommand(0, type, "%s", text);
		}
		else
		{
			SV_SendServerCommand(&mp::svs_clients[client_num], type, "%s", text);
		}
	}

	void Cbuf_AddText(int local_client_num, [[maybe_unused]] int controller_index, const char* cmd)
	{
		if (game::environment::is_sp())
		{
			sp::Cbuf_AddText(local_client_num, cmd);
		}
		else
		{
			mp::Cbuf_AddText(local_client_num, cmd);
		}
	}

	void Cmd_TokenizeString(const char* text)
	{
		if (game::environment::is_sp())
		{
			sp::Cmd_TokenizeString(text);
		}
		else
		{
			mp::Cmd_TokenizeString(text);
		}
	}

	void Cmd_EndTokenizeString()
	{
		if (game::environment::is_sp())
		{
			sp::Cmd_EndTokenizeString();
		}
		else
		{
			mp::Cmd_EndTokenizeString();
		}
	}

	const char* Dvar_ValueToString(dvar_t* dvar, bool is_hashed, dvar_value value)
	{
		if (game::environment::is_sp())
		{
			return sp::Dvar_ValueToString(dvar, is_hashed, value);
		}

		return mp::Dvar_ValueToString(dvar, &value);
	}

	unsigned int SND_GetSoundFileLength(SoundFile* soundfile)
	{
		LoadedSound* sound = nullptr;
		switch (soundfile->type)
		{
		case game::SAT_LOADED:
			sound = soundfile->u.loadSnd;
			break;
		case game::SAT_STREAMED:
			return soundfile->u.streamSnd.totalMsec;
		case game::SAT_PRIMED:
			sound = soundfile->u.primedSnd.loadedPart;
			if (!sound)
				return 0;
			break;
		default:
			return 0;
		}
		if (!sound->info.sampleRate)
			return 0;
		return 1000 * sound->info.numSamples / sound->info.sampleRate;
	}

	unsigned int SND_SV_LookupSoundLength(const char* name)
	{
		game::SoundFile* soundfile = nullptr;
		game::snd_alias_t* soundalias = game::Com_PickSoundAlias(name, 0);
		if (soundalias && (soundfile = soundalias->soundFile) != nullptr)
			return SND_GetSoundFileLength(soundfile);
		return 0;
	}

	namespace environment
	{
		launcher::mode mode = launcher::mode::none;

		launcher::mode translate_surrogate(const launcher::mode _mode)
		{
			switch (_mode)
			{
			case launcher::mode::survival:
			case launcher::mode::zombies:
				return launcher::mode::multiplayer;
			default:
				return _mode;
			}
		}

		launcher::mode get_real_mode()
		{
			if (mode == launcher::mode::none)
			{
				//throw std::runtime_error("Launcher mode not valid. Something must be wrong.");
			}

			return mode;
		}

		launcher::mode get_mode()
		{
			return translate_surrogate(get_real_mode());
		}

		bool is_sp()
		{
			return get_mode() == launcher::mode::singleplayer;
		}

		bool is_mp()
		{
			return get_mode() == launcher::mode::multiplayer;
		}

		bool is_dedi()
		{
			return get_mode() == launcher::mode::server;
		}

		void set_mode(const launcher::mode _mode)
		{
			mode = _mode;
		}

		std::string get_string()
		{
			const auto current_mode = get_real_mode();
			switch (current_mode)
			{
			case launcher::mode::server:
				return "Dedicated Server";

			case launcher::mode::multiplayer:
				return "Multiplayer";

			case launcher::mode::singleplayer:
				return "Singleplayer";

			case launcher::mode::none:
				return "None";

			default:
				return "Unknown (" + std::to_string(static_cast<int>(mode)) + ")";
			}
		}
	}
}

size_t reverse_b(const size_t ptr)
{
	return ptr - game::base_address;
}

size_t reverse_b(const void* ptr)
{
	return reverse_b(reinterpret_cast<size_t>(ptr));
}
