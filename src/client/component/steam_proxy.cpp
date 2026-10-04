#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "steam_proxy.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"

#include "steam/steam.hpp"

#include <utils/flags.hpp>
#include <utils/string.hpp>
#include <utils/binary_resource.hpp>

namespace steam_proxy
{
	namespace
	{
		utils::nt::library steam_client_module_{};

		utils::nt::library steam_overlay_module_{};

		steam::interface client_engine_ {};

		steam::interface client_user_ {};

		steam::interface client_utils_ {};

		void* steam_pipe_ = nullptr;

		void* global_user_ = nullptr;

		bool is_disabled()
		{
			static const auto disabled = utils::flags::has_flag("nosteam");
			return disabled;
		}

		void* load_client_engine()
		{
			if (!steam_client_module_) return nullptr;

			for (auto i = 1; i > 0; ++i)
			{
				std::string name = utils::string::va("CLIENTENGINE_INTERFACE_VERSION%03i", i);
				auto* const client_engine = steam_client_module_
					.invoke<void*>("CreateInterface", name.data(), nullptr);
				if (client_engine) return client_engine;
			}

			return nullptr;
		}

		void load_client()
		{
			const std::filesystem::path steam_path = steam::SteamAPI_GetSteamInstallPath();
			if (steam_path.empty()) 
			{
				return;
			}

			utils::nt::library::load(steam_path / "tier0_s64.dll");
			utils::nt::library::load(steam_path / "vstdlib_s64.dll");
			steam_overlay_module_ = utils::nt::library::load(steam_path / "gameoverlayrenderer64.dll");
			steam_client_module_ = utils::nt::library::load(steam_path / "steamclient64.dll");
			if (!steam_client_module_) return;

			client_engine_ = load_client_engine();
			if (!client_engine_) return;

			steam_pipe_ = steam_client_module_.invoke<void*>("Steam_CreateSteamPipe");
			global_user_ = steam_client_module_.invoke<void*>(
				"Steam_ConnectToGlobalUser", steam_pipe_);
			client_user_ = client_engine_.invoke<void*>(8, steam_pipe_, global_user_); // GetIClientUser
			client_utils_ = client_engine_.invoke<void*>(14, steam_pipe_); // GetIClientUtils
		}

		void start_mod(const std::string& title, size_t app_id)
		{
			if (!client_utils_ || !client_user_) return;

			if (!client_user_.invoke<bool>("BIsSubscribedApp", app_id))
			{
				app_id = 480; // Spacewar
			}

			client_utils_.invoke<void>("SetAppIDForCurrentPipe", app_id, false);
		}

		void clean_up_on_error()
		{
			scheduler::schedule([]()
			{
				if (steam_client_module_
					&& steam_pipe_
					&& global_user_
					&& steam_client_module_.invoke<bool>("Steam_BConnected", global_user_,
						steam_pipe_)
					&& steam_client_module_.invoke<bool>("Steam_BLoggedOn", global_user_, steam_pipe_)
					)
				{
					return scheduler::cond_continue;
				}

				client_engine_ = nullptr;
				client_user_ = nullptr;
				client_utils_ = nullptr;

				steam_pipe_ = nullptr;
				global_user_ = nullptr;

				steam_client_module_ = utils::nt::library{nullptr};

				return scheduler::cond_end;
			});
		}
	}

	const utils::nt::library& get_overlay_module()
	{
		// TODO: Find a better way to do this
		return steam_overlay_module_;
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			if (game::environment::is_dedi() || is_disabled() || !FindWindowA(0, "Steam"))
			{
				return;
			}

			const auto app_id = game::environment::is_sp() ? 393080 : 393100;

			SetEnvironmentVariableA("SteamAppId", utils::string::va("%lu", app_id));
			SetEnvironmentVariableA("SteamGameId", utils::string::va("%llu", app_id & 0xFFFFFF));

			load_client();
			clean_up_on_error();

			try
			{
				start_mod("\xF0\x9F\x8E\xAE" " H1-Mod: "s + (game::environment::is_sp() ? "Singleplayer" : "Multiplayer"), app_id);
			}
			catch (std::exception& e)
			{
				printf("Steam: %s\n", e.what());
			}
		}

		void pre_destroy() override
		{
			if (steam_client_module_)
			{
				if (steam_pipe_)
				{
					if (global_user_)
					{
						steam_client_module_.invoke<void>("Steam_ReleaseUser", steam_pipe_,
							global_user_);
					}

					steam_client_module_.invoke<bool>("Steam_BReleaseSteamPipe", steam_pipe_);
				}
			}
		}
	};
}

REGISTER_COMPONENT(steam_proxy::component)
