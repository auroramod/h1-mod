#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "callstack.hpp"

#include <utils/nt.hpp>

#include <DbgHelp.h>

#pragma comment(lib, "dbghelp.lib")

namespace callstack
{
	namespace
	{
		// dbghelp is not thread safe
		std::recursive_mutex symbol_mutex;
		bool symbols_initialized = false;

		void init_symbols()
		{
			const auto process = GetCurrentProcess();

			if (!symbols_initialized)
			{
				SymSetOptions(SymGetOptions() | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
				symbols_initialized = SymInitialize(process, nullptr, TRUE) == TRUE;
			}

			SymRefreshModuleList(process);
		}

		std::string format_frame(const uint64_t address)
		{
			const auto module = utils::nt::library::get_by_address(reinterpret_cast<void*>(address));
			const auto module_name = module ? module.get_name() : "unknown"s;
			const auto rva = module ? address - reinterpret_cast<uint64_t>(module.get_ptr()) : 0;

			// game binaries are mapped at 0x140000000, so the absolute address matches IDA
			auto result = std::format("{}+0x{:X} (0x{:X})", module_name, rva, address);

			if (!symbols_initialized)
			{
				return result;
			}

			const auto process = GetCurrentProcess();

			char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME]{};
			auto* symbol = reinterpret_cast<SYMBOL_INFO*>(buffer);
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol->MaxNameLen = MAX_SYM_NAME;

			DWORD64 displacement = 0;
			if (SymFromAddr(process, address, &displacement, symbol))
			{
				result.append(std::format(" - {} + 0x{:X}", symbol->Name, displacement));

				IMAGEHLP_LINE64 line{};
				line.SizeOfStruct = sizeof(line);
				DWORD line_displacement = 0;
				if (SymGetLineFromAddr64(process, address, &line_displacement, &line))
				{
					result.append(std::format(" [{}:{}]", line.FileName, line.LineNumber));
				}
			}

			return result;
		}

		void print(const int max_frames = 32)
		{
			void* stack[64]{};
			const auto frames = RtlCaptureStackBackTrace(1, std::min(max_frames, static_cast<int>(ARRAYSIZE(stack))), stack, nullptr);

			std::lock_guard _(symbol_mutex);
			init_symbols();

			for (USHORT i = 0; i < frames; ++i)
			{
				printf("Frame %hu: %s\n", i, format_frame(reinterpret_cast<uint64_t>(stack[i])).data());
			}
		}
	}

	std::string get_summary(const CONTEXT* context, const int max_frames)
	{
		std::string summary("callstack:\r\n{\r\n");

		std::lock_guard _(symbol_mutex);
		init_symbols();

		const auto process = GetCurrentProcess();
		const auto thread = GetCurrentThread();

		// StackWalk64 modifies the context
		auto ctx = *context;

		STACKFRAME64 frame{};
		frame.AddrPC.Offset = ctx.Rip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = ctx.Rbp;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Offset = ctx.Rsp;
		frame.AddrStack.Mode = AddrModeFlat;

		for (auto i = 0; i < max_frames; ++i)
		{
			if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, &frame, &ctx, nullptr,
				SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
			{
				break;
			}

			if (!frame.AddrPC.Offset)
			{
				break;
			}

			summary.append(std::format("\t{:02}: {}\r\n", i, format_frame(frame.AddrPC.Offset)));
		}

		return summary.append("}");
	}

	class component final : public component_interface
	{
	public:
		void pre_destroy() override
		{
			std::lock_guard _(symbol_mutex);
			if (symbols_initialized)
			{
				SymCleanup(GetCurrentProcess());
				symbols_initialized = false;
			}
		}
	};
}

REGISTER_COMPONENT(callstack::component)
