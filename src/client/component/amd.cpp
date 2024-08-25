#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "console.hpp"
#include "command.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

//#include <amd_ags.h>

#pragma pack(push, 1)
struct AGSGPUInfo
{
	char unk[16];
	const char* driver;
	const char* radeon;
	int radeon_version[3];
};
#pragma pack(pop)

enum AGSReturnCode
{
	AGS_SUCCESS,                    ///< Successful function call
	AGS_FAILURE,                    ///< Failed to complete call for some unspecified reason
	AGS_INVALID_ARGS,               ///< Invalid arguments into the function
	AGS_OUT_OF_MEMORY,              ///< Out of memory when allocating space internally
	AGS_MISSING_D3D_DLL,            ///< Returned when a D3D dll fails to load
	AGS_LEGACY_DRIVER,              ///< Returned if a feature is not present in the installed driver
	AGS_NO_AMD_DRIVER_INSTALLED,    ///< Returned if the AMD GPU driver does not appear to be installed
	AGS_EXTENSION_NOT_SUPPORTED,    ///< Returned if the driver does not support the requested driver extension
	AGS_ADL_FAILURE,                ///< Failure in ADL (the AMD Display Library)
	AGS_DX_FAILURE,                 ///< Failure from DirectX runtime
	AGS_D3DDEVICE_NOT_CREATED       ///< Failure due to not creating the D3D device successfully via AGS.
};

struct AGSContext;  ///< All function calls in AGS require a pointer to a context. This is generated via \ref agsInitialize

namespace
{
	//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
	std::string GetLastErrorAsString()
	{
		//Get the error message ID, if any.
		DWORD errorMessageID = ::GetLastError();
		if (errorMessageID == 0) {
			return std::string(); //No error message has been recorded
		}

		LPSTR messageBuffer = nullptr;

		//Ask Win32 to give us the string version of that message ID.
		//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

		//Copy the error message into a std::string.
		std::string message(messageBuffer, size);

		//Free the Win32's string's buffer.
		LocalFree(messageBuffer);

		return message;
	}
}

namespace amd
{
	namespace
	{
		utils::hook::detour agsInit_hook;

		AGSReturnCode agsInitialize(void* config, AGSContext** context, AGSGPUInfo* gpuInfo)
		{
			/*auto lib = LoadLibraryA("atiadlxx.dll");
			if (lib == NULL)
			{
				const auto error_str = GetLastErrorAsString();
				printf("Failed to load library atiadlxy.dll required for AMD AGS.\nERROR:%s\n", error_str.data());
			}
			else
			{
				FARPROC ADL2_Main_Control_Create;
				FARPROC ADL2_Main_Control_Destroy;
				FARPROC ADL2_Adapter_NumberOfAdapters_Get;
				FARPROC ADL2_Adapter_AdapterInfo_Get;
				FARPROC ADL2_Adapter_MemoryInfo_Get;
				FARPROC ADL2_Adapter_Graphic_Core_Info_Get;
				FARPROC ADL2_Overdrive_Caps;
				FARPROC ADL2_Overdrive6_Capabilities_Get;
				FARPROC ADL2_Overdrive6_StateInfo_Get;
				FARPROC ADL2_OverdriveN_Capabilities_Get;
				FARPROC ADL2_OverdriveN_SystemClocks_Get;
				FARPROC ADL2_OverdriveN_MemoryClocks_Get;
				FARPROC ADL2_Graphics_Versions_Get;
				FARPROC ADL2_Graphics_VersionsX2_Get;
				FARPROC ADL2_Display_DisplayMapConfig_Get;
				FARPROC ADL2_Display_SLSMapIndex_Get;
				FARPROC ADL2_Display_SLSMapConfig_Get;
				FARPROC ADL2_Display_Modes_Get;
				FARPROC ADL2_Display_SourceContentAttribute_Get;
				FARPROC ADL2_Display_SourceContentAttribute_Set;
				FARPROC ADL2_Display_DDCInfo2_Get;
				FARPROC ADL2_Display_DisplayInfo_Get;

				ADL2_Main_Control_Create = GetProcAddress(lib, "ADL2_Main_Control_Create");
				ADL2_Main_Control_Destroy = GetProcAddress(lib, "ADL2_Main_Control_Destroy");
				ADL2_Adapter_NumberOfAdapters_Get = GetProcAddress(lib, "ADL2_Adapter_NumberOfAdapters_Get");
				ADL2_Adapter_AdapterInfo_Get = GetProcAddress(lib, "ADL2_Adapter_AdapterInfo_Get");
				ADL2_Adapter_MemoryInfo_Get = GetProcAddress(lib, "ADL2_Adapter_MemoryInfo_Get");
				ADL2_Adapter_Graphic_Core_Info_Get = GetProcAddress(lib, "ADL2_Adapter_Graphic_Core_Info_Get");
				ADL2_Overdrive_Caps = GetProcAddress(lib, "ADL2_Overdrive_Caps");
				ADL2_Overdrive6_Capabilities_Get = GetProcAddress(lib, "ADL2_Overdrive6_Capabilities_Get");
				ADL2_Overdrive6_StateInfo_Get = GetProcAddress(lib, "ADL2_Overdrive6_StateInfo_Get");
				ADL2_OverdriveN_Capabilities_Get = GetProcAddress(lib, "ADL2_OverdriveN_Capabilities_Get");
				ADL2_OverdriveN_SystemClocks_Get = GetProcAddress(lib, "ADL2_OverdriveN_SystemClocks_Get");
				ADL2_OverdriveN_MemoryClocks_Get = GetProcAddress(lib, "ADL2_OverdriveN_MemoryClocks_Get");
				ADL2_Graphics_Versions_Get = GetProcAddress(lib, "ADL2_Graphics_Versions_Get");
				ADL2_Graphics_VersionsX2_Get = GetProcAddress(lib, "ADL2_Graphics_VersionsX2_Get");
				ADL2_Display_DisplayMapConfig_Get = GetProcAddress(lib, "ADL2_Display_DisplayMapConfig_Get");
				ADL2_Display_SLSMapIndex_Get = GetProcAddress(lib, "ADL2_Display_SLSMapIndex_Get");
				ADL2_Display_SLSMapConfig_Get = GetProcAddress(lib, "ADL2_Display_SLSMapConfig_Get");
				ADL2_Display_Modes_Get = GetProcAddress(lib, "ADL2_Display_Modes_Get");
				ADL2_Display_SourceContentAttribute_Get = GetProcAddress(lib, "ADL2_Display_SourceContentAttribute_Get");
				ADL2_Display_SourceContentAttribute_Set = GetProcAddress(lib, "ADL2_Display_SourceContentAttribute_Set");
				ADL2_Display_DDCInfo2_Get = GetProcAddress(lib, "ADL2_Display_DDCInfo2_Get");
				ADL2_Display_DisplayInfo_Get = GetProcAddress(lib, "ADL2_Display_DisplayInfo_Get");
				auto val =
					ADL2_Main_Control_Create
					&& ADL2_Main_Control_Destroy
					&& ADL2_Graphics_Versions_Get
					&& ADL2_Adapter_NumberOfAdapters_Get
					&& ADL2_Adapter_AdapterInfo_Get
					&& ADL2_Adapter_MemoryInfo_Get
					&& ADL2_Display_DisplayMapConfig_Get
					&& ADL2_Display_SLSMapIndex_Get
					&& ADL2_Display_SLSMapConfig_Get
					&& ADL2_Display_Modes_Get;

				printf("t/f: %d\n", val);


				{
					enum adl_res : std::int32_t
					{
						ADL_ERR = -1,
						ADL_OK = 0,
						ADL_OK_WARNING = 1,
					};

					struct ADLVersionsInfoX2
					{
						char strReleaseVersion[256];
						char strCatalystVersion[256];
						char strSoftwareVersion[256];
						char strCatalystWebLink[256];
					};

					int adl_result = ADL_OK;

					typedef void* ADL_CONTEXT_HANDLE;
					ADL_CONTEXT_HANDLE ADL_CONTEXT_HANDLE_;

					adl_result = utils::hook::invoke<int>(ADL2_Main_Control_Create, malloc, 1, &ADL_CONTEXT_HANDLE_);
					if (adl_result == ADL_OK)
					{
						ADLVersionsInfoX2 version_info;
						memset(&version_info, 0, sizeof(ADLVersionsInfoX2));

						auto result1 = utils::hook::invoke<int>(ADL2_Graphics_VersionsX2_Get, ADL_CONTEXT_HANDLE_, &version_info);
						if (result1 > ADL_OK_WARNING)
						{
							printf("err3\n");
						}
						else
						{
							printf("strReleaseVersion: %s\n", version_info.strReleaseVersion);
							printf("strCrimsonVersion: %s\n", version_info.strCatalystVersion);
							printf("strSoftwareVersion: %s\n", version_info.strSoftwareVersion);
							printf("strCatalystWebLink: %s\n", version_info.strCatalystWebLink);
						}

						adl_result = utils::hook::invoke<int>(ADL2_Main_Control_Destroy, ADL_CONTEXT_HANDLE_);
						if (adl_result != ADL_OK)
						{
							printf("err2 %d\n", adl_result);
						}
					}
					else
					{
						printf("err1 %d\n", adl_result);
					}
				}
			}*/

			printf("agsInit\n");
			auto return_code = agsInit_hook.invoke<AGSReturnCode>(config, context, gpuInfo);
			if (return_code)
			{
				printf("ERRRRR\n");
				return return_code;
			}
			if (gpuInfo)
			{
				if (gpuInfo->driver)
				{
					printf("driver: %s\n", gpuInfo->driver);
				}
				if (gpuInfo->radeon)
				{
					printf("radeon: %s\n", gpuInfo->radeon);
				}
				printf("unk[0]: %d\n", gpuInfo->unk[0]);
				printf("radeon_version: %d.%d.%d\n", gpuInfo->radeon_version[0], gpuInfo->radeon_version[1], gpuInfo->radeon_version[2]);
			}
			
			return return_code;
		}

		utils::hook::detour get_amd_gpu_driver_version_hook;

		struct amd_gpu_driver_version
		{
			int version[3];
		};

		amd_gpu_driver_version* get_amd_gpu_driver_version_stub(amd_gpu_driver_version* ver)
		{
			auto result = get_amd_gpu_driver_version_hook.invoke<amd_gpu_driver_version*>(ver);
			if (result->version[0] == 0 && result->version[1] == 0 && result->version[2] == 0)
			{
				// something went wrong, happens with new amd gpus. just give a good result i guess. AMD RADEON SOFTWARE REVISION NUMBER(16.11.2)
				result->version[0] = 16;
				result->version[1] = 11;
				result->version[2] = 2;
#ifdef DEBUG
				std::string gpu_name = "Unknown GPU";
				const auto* dvar = game::Dvar_FindVar("sys_gpu");
				if (dvar && dvar->current.string)
				{
					gpu_name = dvar->current.string;
				}
				printf("Returning AMD RADEON SOFTWARE REVISION NUMBER(16.11.2) for AMD gpu: %s\n", gpu_name.data());
#endif
			}
			return result;
		}
	}

	utils::hook::detour test_hook;
	bool test_stub(__int64 a1)
	{
		auto result = test_hook.invoke<bool>(a1);
		printf("%d\n", result);
		return result;
	}

	class component final : public component_interface
	{
	public:
		void* load_import(const std::string& library, const std::string& function) override
		{
			//if (library == "amd_ags_x64.dll")
			//{
			//	if (function == "agsInit") return agsInitialize;
			//}

			return nullptr;
		}

		void post_unpack() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			// todo: sp support
			agsInit_hook.create(0x717908_b, agsInitialize);
			get_amd_gpu_driver_version_hook.create(0x688B10_b, get_amd_gpu_driver_version_stub);
			//
			command::add("amd_revision", []
			{
				const auto* is_amd_gpu = game::Dvar_FindVar("r_amdGPU");
				if (is_amd_gpu && is_amd_gpu->current.enabled)
				{
					int version[3];
					utils::hook::invoke<int*>(0x688B10_b, version);

					console::info("revision: %d.%d.%d", version[0], version[1], version[2]);
				}
			});
		}
	};
}

REGISTER_COMPONENT(amd::component)