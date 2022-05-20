#include <std_include.hpp>
#include "console.hpp"
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "command.hpp"

#include <utils/thread.hpp>
#include <utils/hook.hpp>

namespace game_console
{
	void print(int type, const std::string& data);
}

namespace console
{
	namespace
	{
		bool kill = false;
		std::thread console_thread;
	}

	std::string format(va_list* ap, const char* message)
	{
		static thread_local char buffer[0x1000];

		const auto count = _vsnprintf_s(buffer, sizeof(buffer), sizeof(buffer), message, *ap);

		if (count < 0) return {};
		return {buffer, static_cast<size_t>(count)};
	}

	void dispatch_message(const int type, const std::string& message)
	{
		printf("%s", message.data());
		game_console::print(type, message);
	}

	void print(const int type, const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		const auto result = format(&ap, fmt);
		va_end(ap);

		dispatch_message(type, result);
	}

	class component final : public component_interface
	{
	public:
		component()
		{
			ShowWindow(GetConsoleWindow(), SW_HIDE);
		}

		void post_unpack() override
		{
			ShowWindow(GetConsoleWindow(), SW_SHOW);
			SetConsoleTitle("H1-Mod");

			console_thread = utils::thread::create_named_thread("Console", []()
			{
				std::string cmd;

				while (!kill)
				{
					std::getline(std::cin, cmd);
					game::Cbuf_AddText(0, 0, cmd.data());
				}
			});
		}

		void pre_destroy() override
		{
			kill = true;

			if (console_thread.joinable())
			{
				console_thread.join();
			}
		}
	};
}

REGISTER_COMPONENT(console::component)
