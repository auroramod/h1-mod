#include <std_include.hpp>
#include "console.hpp"
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "command.hpp"

namespace game_console
{
	void print(int type, const std::string& data);
}

namespace console
{
	namespace
	{
		static bool ingame = false;

		DWORD WINAPI console(LPVOID)
		{
			ShowWindow(GetConsoleWindow(), SW_SHOW);
			SetConsoleTitle("H1-Mod");

			std::string cmd;

			while (true)
			{
				std::getline(std::cin, cmd);
				if (ingame)
				{
					game::Cbuf_AddText(0, 0, cmd.data());
				}
			}

			return 0;
		}
	}

	std::string format(va_list* ap, const char* message)
	{
		static thread_local char buffer[0x1000];

		const auto count = _vsnprintf_s(buffer, sizeof(buffer), sizeof(buffer), message, *ap);

		if (count < 0) return {};
		return { buffer, static_cast<size_t>(count) };
	}

	void dispatch_message(const int type, const std::string& message)
	{
		printf("%s\n", message.data());

		//game_console::print(type, message);
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
		void post_start() override
		{
			CreateThread(0, 0, console, 0, 0, 0);
		}

		void post_unpack() override
		{
			ingame = true;
		}

		void pre_destroy() override
		{
			ingame = false;
		}
	};
}

REGISTER_COMPONENT(console::component)
