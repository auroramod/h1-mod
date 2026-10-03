#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "exception.hpp"

#include "callstack.hpp"
#include "scheduler.hpp"
#include "system_check.hpp"
#include "version.hpp"

#include "game/dvars.hpp"
#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/nt.hpp>
#include <utils/string.hpp>
#include <utils/thread.hpp>

#include <exception/minidump.hpp>

static thread_local struct
{
	DWORD code = 0;
	PVOID address = nullptr;
} exception_data;

static struct
{
	std::chrono::time_point<std::chrono::high_resolution_clock> last_recovery{};
	std::atomic<int> recovery_counts = {0};
} recovery_data;

exception_component::exception_component()
{
	SetUnhandledExceptionFilter(exception_filter);
}

void exception_component::post_load()
{
	SetUnhandledExceptionFilter(exception_filter);
	utils::hook::jump(SetUnhandledExceptionFilter, set_unhandled_exception_filter_stub, true);

	scheduler::on_game_initialized([]
	{
		is_initialized() = true;
	});
}

void exception_component::post_unpack()
{
	dvars::cg_legacyCrashHandling = dvars::register_bool("cg_legacyCrashHandling",
		false, game::DVAR_ARCHIVE, "Toggle new crash handling");
}

bool exception_component::is_game_thread()
{
	static std::vector<int> allowed_threads =
	{
		game::THREAD_CONTEXT_MAIN,
	};

	const auto self_id = GetCurrentThreadId();
	for (const auto& index : allowed_threads)
	{
		if (game::threadIds[index] == self_id)
		{
			return true;
		}
	}

	return false;
}

bool exception_component::is_exception_interval_too_short()
{
	const auto delta = std::chrono::high_resolution_clock::now() - recovery_data.last_recovery;
	return delta < 1min;
}

bool exception_component::too_many_exceptions_occured()
{
	return recovery_data.recovery_counts >= 3;
}

volatile bool& exception_component::is_initialized()
{
	static volatile bool initialized = false;
	return initialized;
}

bool exception_component::is_recoverable()
{
	return is_initialized()
		&& is_game_thread()
		&& !is_exception_interval_too_short()
		&& !too_many_exceptions_occured();
}

void exception_component::show_mouse_cursor()
{
	while (ShowCursor(TRUE) < 0);
}

void exception_component::display_error_dialog()
{
	std::string error_str = utils::string::va("Fatal error (0x%08X) at 0x%p (0x%p).\n"
	                                          "A minidump has been written.\n\n",
	                                          exception_data.code, exception_data.address, 
	                                          reinterpret_cast<uint64_t>(exception_data.address) - game::base_address);

	if (!system_check::is_valid())
	{
		error_str += "Make sure to get supported game files to avoid such crashes!";
	}
	else
	{
		error_str += "Make sure to update your graphics card drivers and install operating system updates!";
	}

	utils::thread::suspend_other_threads();
	show_mouse_cursor();

	MSG_BOX_ERROR(error_str.data());
	TerminateProcess(GetCurrentProcess(), exception_data.code);
}

void exception_component::reset_state()
{
	if (dvars::cg_legacyCrashHandling && dvars::cg_legacyCrashHandling->current.enabled)
	{
		display_error_dialog();
	}

	// TODO: Add a limit for dedi restarts
	if (game::environment::is_dedi())
	{
		utils::nt::relaunch_self();
		utils::nt::terminate(exception_data.code);
	}

	if (is_recoverable())
	{
		recovery_data.last_recovery = std::chrono::high_resolution_clock::now();
		++recovery_data.recovery_counts;

		game::Com_Error(game::ERR_DROP, "Fatal error (0x%08X) at 0x%p.\nA minidump has been written.\n\n"
		                "H1-Mod has tried to recover your game, but it might not run stable anymore.\n\n"
		                "Make sure to update your graphics card drivers and install operating system updates!\n"
		                "Closing or restarting Steam might also help.",
		                exception_data.code, exception_data.address);
	}
	else
	{
		display_error_dialog();
	}
}

size_t exception_component::get_reset_state_stub()
{
	static auto* stub = utils::hook::assemble([](utils::hook::assembler& a)
	{
		a.sub(rsp, 0x10);
		a.or_(rsp, 0x8);
		a.jmp(reset_state);
	});

	return reinterpret_cast<size_t>(stub);
}

std::string exception_component::get_timestamp()
{
	tm ltime{};
	char timestamp[MAX_PATH] = {0};
	const auto time = _time64(nullptr);

	_localtime64_s(&ltime, &time);
	strftime(timestamp, sizeof(timestamp) - 1, "%Y-%m-%d-%H-%M-%S", &ltime);

	return timestamp;
}

std::string exception_component::generate_crash_info(const LPEXCEPTION_POINTERS exceptioninfo)
{
	std::string info{};
	const auto line = [&info](const std::string& text)
	{
		info.append(text);
		info.append("\r\n");
	};

	line("H1-Mod Crash Dump");
	line("");
	line("Version: "s + VERSION);
	line("Environment: "s + game::environment::get_string());
	line("Timestamp: "s + get_timestamp());
	line("Clean game: "s + (system_check::is_valid() ? "Yes" : "No"));
	line("OS: "s + (utils::nt::is_wine() ? "wine" : "windows"));
	line("");
	line(utils::string::va("Exception: 0x%08X (%s)", exceptioninfo->ExceptionRecord->ExceptionCode,
		get_exception_string(exceptioninfo->ExceptionRecord->ExceptionCode)));
	line(utils::string::va("Address: 0x%llX [%s]", exceptioninfo->ExceptionRecord->ExceptionAddress,
		utils::nt::library::get_by_address(exceptioninfo->ExceptionRecord->ExceptionAddress).get_name().data()));
	line(utils::string::va("Base: 0x%llX", game::base_address));
	line(utils::string::va("Main Module: %s [0x%llX]", utils::nt::library{}.get_name().data(), utils::nt::library{}.get_ptr()));
	line(utils::string::va("Thread ID: %d (%s)", GetCurrentThreadId(), is_game_thread() ? "Main Thread" : "Auxiliary Thread"));

	if (exceptioninfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
	{
		const auto access = exceptioninfo->ExceptionRecord->ExceptionInformation[0];
		line(utils::string::va("Extended Info: Attempted to %s 0x%llX",
			access == 1 ? "write to" : (access == 8 ? "execute" : "read from"),
			exceptioninfo->ExceptionRecord->ExceptionInformation[1]));
	}

#pragma warning(push)
#pragma warning(disable: 4996)
	OSVERSIONINFOEXA version_info;
	ZeroMemory(&version_info, sizeof(version_info));
	version_info.dwOSVersionInfoSize = sizeof(version_info);
	GetVersionExA(reinterpret_cast<LPOSVERSIONINFOA>(&version_info));
#pragma warning(pop)

	line(utils::string::va("OS Version: %u.%u", version_info.dwMajorVersion, version_info.dwMinorVersion));

	line("");
	line(callstack::get_summary(exceptioninfo->ContextRecord));
	line(get_memory_registers(exceptioninfo));

	return info;
}

const char* exception_component::get_exception_string(const DWORD exception)
{
#define EXCEPTION_CASE(CODE) case EXCEPTION_##CODE: return "EXCEPTION_" #CODE
	switch (exception)
	{
		EXCEPTION_CASE(ACCESS_VIOLATION);
		EXCEPTION_CASE(DATATYPE_MISALIGNMENT);
		EXCEPTION_CASE(BREAKPOINT);
		EXCEPTION_CASE(SINGLE_STEP);
		EXCEPTION_CASE(ARRAY_BOUNDS_EXCEEDED);
		EXCEPTION_CASE(FLT_DENORMAL_OPERAND);
		EXCEPTION_CASE(FLT_DIVIDE_BY_ZERO);
		EXCEPTION_CASE(FLT_INEXACT_RESULT);
		EXCEPTION_CASE(FLT_INVALID_OPERATION);
		EXCEPTION_CASE(FLT_OVERFLOW);
		EXCEPTION_CASE(FLT_STACK_CHECK);
		EXCEPTION_CASE(FLT_UNDERFLOW);
		EXCEPTION_CASE(INT_DIVIDE_BY_ZERO);
		EXCEPTION_CASE(INT_OVERFLOW);
		EXCEPTION_CASE(PRIV_INSTRUCTION);
		EXCEPTION_CASE(IN_PAGE_ERROR);
		EXCEPTION_CASE(ILLEGAL_INSTRUCTION);
		EXCEPTION_CASE(NONCONTINUABLE_EXCEPTION);
		EXCEPTION_CASE(STACK_OVERFLOW);
		EXCEPTION_CASE(INVALID_DISPOSITION);
		EXCEPTION_CASE(GUARD_PAGE);
		EXCEPTION_CASE(INVALID_HANDLE);
	default:
		return "UNKNOWN";
	}
#undef EXCEPTION_CASE
}

std::string exception_component::get_memory_registers(const LPEXCEPTION_POINTERS exceptioninfo)
{
	const auto* ctx = exceptioninfo->ContextRecord;
	if (!ctx)
	{
		return {};
	}

	std::string registers("registers:\r\n{\r\n");
	const auto add = [&registers](const char* key, const DWORD64 value)
	{
		registers.append(utils::string::va("\t%s = 0x%llX\r\n", key, value));
	};

	add("rax", ctx->Rax);
	add("rbx", ctx->Rbx);
	add("rcx", ctx->Rcx);
	add("rdx", ctx->Rdx);
	add("rsp", ctx->Rsp);
	add("rbp", ctx->Rbp);
	add("rsi", ctx->Rsi);
	add("rdi", ctx->Rdi);
	add("r8", ctx->R8);
	add("r9", ctx->R9);
	add("r10", ctx->R10);
	add("r11", ctx->R11);
	add("r12", ctx->R12);
	add("r13", ctx->R13);
	add("r14", ctx->R14);
	add("r15", ctx->R15);
	add("rip", ctx->Rip);

	return registers.append("}");
}

void exception_component::write_minidump(const LPEXCEPTION_POINTERS exceptioninfo)
{
	const std::string crash_dir = utils::string::va("minidumps/h1-mod-crash-%d-%s",
	                                                game::environment::get_real_mode(),
	                                                get_timestamp().data());

	utils::io::write_file(crash_dir + "/crash.dmp", exception::create_minidump(exceptioninfo));
	utils::io::write_file(crash_dir + "/info.txt", generate_crash_info(exceptioninfo));
}

bool exception_component::is_harmless_error(const LPEXCEPTION_POINTERS exceptioninfo)
{
	const auto code = exceptioninfo->ExceptionRecord->ExceptionCode;
	return code == STATUS_INTEGER_OVERFLOW || code == STATUS_FLOAT_OVERFLOW || code == STATUS_SINGLE_STEP;
}

LONG WINAPI exception_component::exception_filter(const LPEXCEPTION_POINTERS exceptioninfo)
{
	if (is_harmless_error(exceptioninfo))
	{
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	write_minidump(exceptioninfo);

	exception_data.code = exceptioninfo->ExceptionRecord->ExceptionCode;
	exception_data.address = exceptioninfo->ExceptionRecord->ExceptionAddress;
	exceptioninfo->ContextRecord->Rip = get_reset_state_stub();

	return EXCEPTION_CONTINUE_EXECUTION;
}

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI exception_component::set_unhandled_exception_filter_stub(LPTOP_LEVEL_EXCEPTION_FILTER)
{
	// Don't register anything here...
	return &exception_filter;
}

REGISTER_COMPONENT(exception_component)
