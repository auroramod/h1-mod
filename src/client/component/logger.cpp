#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "party.hpp"
#include "console.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace logger
{
	namespace
	{
		utils::hook::detour com_error_hook;

		void print_error(const char* msg, ...)
		{
			char buffer[2048];

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

			va_end(ap);

			console::error(buffer);
		}

		void print_com_error(int, const char* msg, ...)
		{
			char buffer[2048];

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

			va_end(ap);

			console::error(buffer);
		}

		void com_error_stub(const int error, const char* msg, ...)
		{
			char buffer[2048];

			{
				va_list ap;
				va_start(ap, msg);

				vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

				va_end(ap);

				console::error("Error: %s\n", buffer);
			}

			party::clear_sv_motd(); // clear sv_motd on error if it exists

			com_error_hook.invoke<void>(error, "%s", buffer);
		}

		void print_warning(const char* msg, ...)
		{
			char buffer[2048];

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

			va_end(ap);

			console::warn(buffer);
		}

		void print(const char* msg, ...)
		{
			char buffer[2048];

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

			va_end(ap);

			console::info(buffer);
		}

		void print_dev(const char* msg, ...)
		{
			static auto* enabled = dvars::register_bool("logger_dev", false, game::DVAR_FLAG_SAVED, "Print dev stuff");
			if (!enabled->current.enabled)
			{
				return;
			}

			char buffer[2048];

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

			va_end(ap);

			console::info(buffer);
		}

		void lui_error()
		{
			utils::hook::call(0x140162809, print_warning);
			utils::hook::call(0x140162815, print_warning);
			utils::hook::call(0x14016281D, print_warning);
			utils::hook::call(0x140162829, print_warning);

			utils::hook::call(0x140162E32, print_warning);
			utils::hook::call(0x140162E3E, print_warning);
			utils::hook::call(0x140162E46, print_warning);
			utils::hook::call(0x140162E52, print_warning);

			utils::hook::call(0x140168435, print_warning);
			utils::hook::call(0x140168441, print_warning);
			utils::hook::call(0x140168449, print_warning);
			utils::hook::call(0x140168455, print_warning);

			utils::hook::call(0x14016914D, print_warning);
			utils::hook::call(0x140169161, print_warning);

			utils::hook::call(0x140169C04, print_warning);
			utils::hook::call(0x140169C0C, print_warning);
			utils::hook::call(0x140169C18, print_warning); 

			utils::hook::call(0x140169CB7, print_warning);
			utils::hook::call(0x140169CDE, print_warning);
			utils::hook::call(0x140169CEA, print_warning);
			utils::hook::call(0x140169D03, print_warning);

			utils::hook::call(0x14016BE72, print_warning);
			utils::hook::call(0x14016C020, print_warning);
		}

		void lui_interface_debug_print()
		{
			utils::hook::call(0x14015C0B2, print_warning);
			utils::hook::call(0x140162453, print_warning);
			utils::hook::call(0x1401625DF, print_warning);
			utils::hook::call(0x14016713C, print_dev);
			utils::hook::call(0x1401687CD, print_dev);
			utils::hook::call(0x14016BB8A, print_dev);
			utils::hook::call(0x14016CA9C, print_dev);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_mp())
			{
				lui_error();
				lui_interface_debug_print();
			}

			if (!game::environment::is_sp())
			{
				utils::hook::call(0x14051347F, print_com_error);
			}

			com_error_hook.create(game::Com_Error, com_error_stub);
		}
	};
}

REGISTER_COMPONENT(logger::component)
