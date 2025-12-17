#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

namespace wmi
{
	namespace
	{
		HRESULT WINAPI co_initialize_ex_stub(LPVOID pvReserved, DWORD dwCoInit)
		{
			if ((uint64_t)_ReturnAddress() == SELECT_VALUE(0x6CF89E_b, 0x81A3BE_b))
			{
				return E_FAIL;
			}
			
			return CoInitializeEx(pvReserved, dwCoInit);
		}
	}

	class component final : public component_interface
	{
	public:
		void* load_import(const std::string& library, const std::string& function) override
		{
			if (function == "CoInitializeEx")
			{
				return co_initialize_ex_stub;
			}

			return nullptr;
		}

		void post_unpack() override
		{
			// disable WMI and remove Hardware Query (uses WMI)
			utils::hook::set<uint8_t>(SELECT_VALUE(0x450C0_b, 0x4D360_b), 0xC3); // WMI
			utils::hook::set<uint8_t>(SELECT_VALUE(0x311790_b, 0xB6D20_b), 0xC3); // Hardware query
		}
	};
}

REGISTER_COMPONENT(wmi::component)