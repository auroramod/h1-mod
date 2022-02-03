#include <std_include.hpp>

#include <utils/nt.hpp>
#include <utils/hook.hpp>

#include "seh.hpp"

namespace seh
{
	namespace
	{
		void*(*rtlpx_lookup_function_table)(void*, FUNCTION_TABLE_DATA*);
		void*(*rtlpx_lookup_function_table_down_level)(void*, PDWORD64, PULONG);

		FUNCTION_TABLE_DATA overridden_table;

		DWORD64 override_end;
		DWORD64 override_start;

		void* find_call_from_address(void* method_ptr, ud_mnemonic_code mnemonic = UD_Icall)
		{
			ud_t ud;
			ud_init(&ud);
			ud_set_mode(&ud, 64);
			ud_set_pc(&ud, reinterpret_cast<uint64_t>(method_ptr));
			ud_set_input_buffer(&ud, static_cast<uint8_t*>(method_ptr), INT32_MAX);

			void* retval = nullptr;
			while (true)
			{
				ud_disassemble(&ud);

				if (ud_insn_mnemonic(&ud) == UD_Iint3) break;
				if (ud_insn_mnemonic(&ud) == mnemonic)
				{
					const auto* const operand = ud_insn_opr(&ud, 0);
					if (operand->type == UD_OP_JIMM)
					{
						if (!retval) retval = reinterpret_cast<void*>(ud_insn_len(&ud) + ud_insn_off(&ud) + operand->
							lval.sdword);
						else
						{
							retval = nullptr;
							break;
						}
					}
				}
			}

			return retval;
		}

		void* rtlpx_lookup_function_table_override(void* exception_address, FUNCTION_TABLE_DATA* out_data)
		{
			ZeroMemory(out_data, sizeof(*out_data));

			auto* retval = seh::rtlpx_lookup_function_table(exception_address, out_data);

			const auto address_num = DWORD64(exception_address);
			if (address_num >= seh::override_start && address_num <= seh::override_end)
			{
				if (address_num != 0)
				{
					*out_data = seh::overridden_table;
					retval = PVOID(seh::overridden_table.TableAddress);
				}
			}

			return retval;
		}

		void* rtlpx_lookup_function_table_override_down_level(void* exception_address, const PDWORD64 image_base,
		                                                      const PULONG length)
		{
			auto* retval = seh::rtlpx_lookup_function_table_down_level(exception_address, image_base, length);

			const auto address_num = DWORD64(exception_address);
			if (address_num >= seh::override_start && address_num <= seh::override_end)
			{
				if (address_num != 0)
				{
					*image_base = seh::overridden_table.ImageBase;
					*length = seh::overridden_table.Size;

					retval = PVOID(seh::overridden_table.TableAddress);
				}
			}

			return retval;
		}
	}

	void setup_handler(void* module_base, void* module_end, PRUNTIME_FUNCTION runtime_functions, const DWORD entryCount)
	{
		const utils::nt::library ntdll("ntdll.dll");

		seh::override_start = DWORD64(module_base);
		seh::override_end = DWORD64(module_end);

		seh::overridden_table.ImageBase = seh::override_start;
		seh::overridden_table.TableAddress = DWORD64(runtime_functions);
		seh::overridden_table.Size = entryCount * sizeof(RUNTIME_FUNCTION);

		if (IsWindows8Point1OrGreater())
		{
			struct
			{
				DWORD64 field0;
				DWORD imageSize;
				DWORD fieldC;
				DWORD64 field10;
			} query_result = {0, 0, 0, 0};

			ntdll.invoke_pascal<NTSTATUS>("NtQueryVirtualMemory", GetCurrentProcess(), module_base, 6, &query_result,
			                              sizeof(query_result), nullptr);
			seh::overridden_table.ImageSize = query_result.imageSize;
		}

		auto* base_address = ntdll.get_proc<void*>("RtlLookupFunctionTable");
		auto* internal_address = find_call_from_address(base_address);

		void* patch_function = rtlpx_lookup_function_table_override;
		auto** patch_original = reinterpret_cast<void**>(&seh::rtlpx_lookup_function_table);

		if (!internal_address)
		{
			if (!IsWindows8Point1OrGreater())
			{
				internal_address = find_call_from_address(base_address, UD_Ijmp);
				patch_function = rtlpx_lookup_function_table_override_down_level;
				patch_original = reinterpret_cast<void**>(&seh::rtlpx_lookup_function_table_down_level);
			}

			if (!internal_address)
			{
				if (IsWindows8OrGreater())
				{
					// TODO: Catch the error
				}

				internal_address = base_address;
				patch_function = rtlpx_lookup_function_table_override_down_level;
				patch_original = reinterpret_cast<void**>(&seh::rtlpx_lookup_function_table_down_level);
			}
		}

		static utils::hook::detour hook{};
		hook = utils::hook::detour(internal_address, patch_function);
		*patch_original = hook.get_original();
	}
}
