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
		DWORD WINAPI console(LPVOID)
		{
			FreeConsole();
			AllocConsole();
			AttachConsole(GetCurrentProcessId());

			(void)freopen("CONIN$", "r", stdin);
			(void)freopen("CONOUT$", "w", stdout);

			std::string cmd;

			while (true)
			{
				std::getline(std::cin, cmd);
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
		//messages.access([&message](message_queue& msgs)
		//	{
		//		msgs.emplace(message);
		//	});
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
	};
}

REGISTER_COMPONENT(console::component)
