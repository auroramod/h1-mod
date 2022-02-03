#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "debugger.hpp"

namespace
{
	bool acquire_debug_privilege()
	{
		TOKEN_PRIVILEGES token_privileges;
		ZeroMemory(&token_privileges, sizeof(token_privileges));
		token_privileges.PrivilegeCount = 1;

		HANDLE token;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &token))
		{
			return false;
		}

		if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &token_privileges.Privileges[0].Luid))
		{
			CloseHandle(token);
			return false;
		}

		token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		DWORD size;
		if (!AdjustTokenPrivileges(token, FALSE, &token_privileges, 0, nullptr, &size))
		{
			CloseHandle(token);
			return false;
		}

		return CloseHandle(token) != FALSE;
	}
}

debugger::debugger(const unsigned long process_id, const bool start)
{
	if (!start)
	{
		return;
	}

	this->runner_ = std::thread([this, process_id]()
	{
		this->terminate_ = false;
		this->run(process_id);
	});
}

debugger::~debugger()
{
	this->terminate_ = true;
	if (this->runner_.joinable())
	{
		this->runner_.join();
	}
}

void debugger::run(const unsigned long process_id) const
{
	acquire_debug_privilege();
	if (!DebugActiveProcess(process_id))
	{
		return;
	}

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

	DEBUG_EVENT event;
	while (!this->terminate_ && WaitForDebugEvent(&event,INFINITE))
	{
		if (event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
		{
			ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_EXCEPTION_NOT_HANDLED);
			continue;
		}

		if (event.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
		{
			ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
			break;
		}

#ifdef DEV_BUILD
		if (event.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT)
		{
			OutputDebugStringA("Debugger attached!\n");
		}
#endif

		ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
	}
}
