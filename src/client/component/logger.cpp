#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "logger.hpp"
#include "game/game.hpp"
#include "game/dvars.hpp"

#include "party.hpp"
#include "console.hpp"

#include <utils/hook.hpp>

static utils::hook::detour com_error_hook;

static game::dvar_t* logger_dev = nullptr;
static game::dvar_t* r_warnings_enable = nullptr;

void logger::post_unpack()
{
	if (!game::environment::is_dedi())
	{
		// lua stuff
		utils::hook::jump(SELECT_VALUE(0x140106010, 0x140178DC0), print_dev);   // debug
		utils::hook::jump(SELECT_VALUE(0x140107680, 0x14017A410), print_error); // error
		utils::hook::jump(SELECT_VALUE(0x1400E6E30, 0x14015ADE0), print);      	// print

		if (game::environment::is_mp())
		{
			utils::hook::call(0x1406133B1, r_warn_once_per_frame_vsnprintf_stub);

			utils::hook::jump(0x1403C2A20, print_warning); // dmWarn
			utils::hook::jump(0x1403C2930, print); // dmLog

			r_warnings_enable = dvars::register_bool("r_enableWarnings", false, game::DVAR_ARCHIVE, "enable rendering warnings");
		}
	}

	com_error_hook.create(game::Com_Error, com_error_stub);

	logger_dev = dvars::register_bool("logger_dev", false, game::DVAR_ARCHIVE, "Print dev stuff");
}

void logger::print_error(const char* msg, ...)
{
	char buffer[2048]{};
	va_list ap;

	va_start(ap, msg);
	vsnprintf_s(buffer, _TRUNCATE, msg, ap);
	va_end(ap);

	console::error("%s", buffer);
}

void logger::print_com_error(int, const char* msg, ...)
{
	char buffer[2048]{};
	va_list ap;

	va_start(ap, msg);
	vsnprintf_s(buffer, _TRUNCATE, msg, ap);
	va_end(ap);

	console::error("%s", buffer);
}

void logger::com_error_stub(const int error, const char* msg, ...)
{
	char buffer[2048]{};
	va_list ap;

	va_start(ap, msg);
	vsnprintf_s(buffer, _TRUNCATE, msg, ap);
	va_end(ap);

	console::error("Error: %s\n", buffer);
#ifdef _DEBUG
	console::error("Com_Error called from 0x%p\n", _ReturnAddress());
#endif

	party::clear_sv_motd(); // clear sv_motd on error if it exists

	com_error_hook.invoke<void>(error, "%s", buffer);
}

void logger::print_warning(const char* msg, ...)
{
	char buffer[2048]{};
	va_list ap;

	va_start(ap, msg);
	vsnprintf_s(buffer, _TRUNCATE, msg, ap);
	va_end(ap);

	console::warn("%s", buffer);
}

void logger::print(const char* msg, ...)
{
	char buffer[2048]{};
	va_list ap;

	va_start(ap, msg);
	vsnprintf_s(buffer, _TRUNCATE, msg, ap);
	va_end(ap);

	console::info("%s", buffer);
}

void logger::print_dev(const char* msg, ...)
{
	if (!logger_dev->current.enabled)
	{
		return;
	}

	char buffer[2048]{};
	va_list ap;

	va_start(ap, msg);
	vsnprintf_s(buffer, _TRUNCATE, msg, ap);
	va_end(ap);

	console::info("%s", buffer);
}

void logger::r_warn_once_per_frame_vsnprintf_stub(char* buffer, size_t buffer_length, char* msg, va_list va)
{
	if (!r_warnings_enable->current.enabled)
	{
		return;
	}

	vsnprintf(buffer, buffer_length, msg, va);
	console::warn(buffer);
}

REGISTER_COMPONENT(logger)
