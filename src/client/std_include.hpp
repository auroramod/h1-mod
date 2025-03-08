#pragma once

#define BINARY_PAYLOAD_SIZE 0x14000000

// Decide whether to load the game as lib or to inject it
#define INJECT_HOST_AS_LIB

#pragma warning(push)
#pragma warning(disable: 4100)
#pragma warning(disable: 4127)
#pragma warning(disable: 4244)
#pragma warning(disable: 4458)
#pragma warning(disable: 4702)
#pragma warning(disable: 4996)
#pragma warning(disable: 5054)
#pragma warning(disable: 6011)
#pragma warning(disable: 6297)
#pragma warning(disable: 6385)
#pragma warning(disable: 6386)
#pragma warning(disable: 6387)
#pragma warning(disable: 26110)
#pragma warning(disable: 26451)
#pragma warning(disable: 26444)
#pragma warning(disable: 26451)
#pragma warning(disable: 26489)
#pragma warning(disable: 26495)
#pragma warning(disable: 26498)
#pragma warning(disable: 26812)
#pragma warning(disable: 28020)

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <MsHTML.h>
#include <MsHtmHst.h>
#include <ExDisp.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <corecrt_io.h>
#include <fcntl.h>
#include <shellapi.h>
#include <csetjmp>
#include <ShlObj.h>
#include <winternl.h>
#include <VersionHelpers.h>
#include <Psapi.h>
#include <urlmon.h>
#include <atlbase.h>
#include <iphlpapi.h>
#include <wincrypt.h>
#include <shellscalingapi.h>
#include <dwmapi.h>

// min and max is required by gdi, therefore NOMINMAX won't work
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#ifdef DEBUG
//#define DW_DEBUG
#endif

#define MSG_BOX_INFO(message) MessageBoxA(nullptr, message, "H1-Mod: INFORMATION", MB_ICONINFORMATION);
#define MSG_BOX_WARN(message) MessageBoxA(nullptr, message, "H1-Mod: WARNING", MB_ICONWARNING);
#define MSG_BOX_ERROR(message) MessageBoxA(nullptr, message, "H1-Mod: ERROR", MB_ICONERROR);

#include <map>
#include <atomic>
#include <vector>
#include <mutex>
#include <queue>
#include <regex>
#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>
#include <utility>
#include <filesystem>
#include <functional>
#include <sstream>
#include <optional>
#include <unordered_set>
#include <variant>
#include <random>

#include <gsl/gsl>
#include <udis86.h>
#include <MinHook.h>
#include <tomcrypt.h>

#pragma warning(push)
#pragma warning(disable: 4459)
#include <json.hpp>
#pragma warning(pop)

#define RAPIDJSON_NOEXCEPT
#define RAPIDJSON_ASSERT(cond) if(cond); else throw std::runtime_error("rapidjson assert fail");

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include <asmjit/core/jitruntime.h>
#include <asmjit/x86/x86assembler.h>

#include <google/protobuf/stubs/logging.h>
#include <proto/auth.pb.h>

#include <d3d11.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <misc/cpp/imgui_stdlib.h>

#pragma warning(pop)
#pragma warning(disable: 4100)

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "urlmon.lib" )
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "d3d11.lib")

#include "resource.hpp"

using namespace std::literals;
