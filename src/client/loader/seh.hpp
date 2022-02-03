#pragma once

struct FUNCTION_TABLE_DATA
{
	DWORD64 TableAddress;
	DWORD64 ImageBase;
	DWORD ImageSize; // field +8 in ZwQueryVirtualMemory class 6
	DWORD Size;
};

typedef enum _FUNCTION_TABLE_TYPE
{
	RF_SORTED,
	RF_UNSORTED,
	RF_CALLBACK
} FUNCTION_TABLE_TYPE;

typedef struct _DYNAMIC_FUNCTION_TABLE
{
	LIST_ENTRY Links;
	PRUNTIME_FUNCTION FunctionTable;
	LARGE_INTEGER TimeStamp;

	ULONG_PTR MinimumAddress;
	ULONG_PTR MaximumAddress;
	ULONG_PTR BaseAddress;

	PGET_RUNTIME_FUNCTION_CALLBACK Callback;
	PVOID Context;
	PWSTR OutOfProcessCallbackDll;
	FUNCTION_TABLE_TYPE Type;
	ULONG EntryCount;
} DYNAMIC_FUNCTION_TABLE, *PDYNAMIC_FUNCTION_TABLE;

namespace seh
{
	void setup_handler(void* module_base, void* module_end, PRUNTIME_FUNCTION runtime_functions, DWORD entryCount);
}
