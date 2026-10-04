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
			if (reinterpret_cast<size_t>(_ReturnAddress()) == static_cast<size_t>(SELECT_VALUE(0x1406CF89E, 0x14076DFA6)))
			{
				return E_FAIL;
			}

			return CoInitializeEx(pvReserved, dwCoInit);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			if (game::environment::is_sp())
			{
				return;
			}

			// disable WMI and remove Hardware Query (uses WMI)
			utils::hook::set<uint8_t>(0x140046588, 0xC3); // WMI
			utils::hook::set<uint8_t>(0x14009CA40, 0xC3); // Hardware query
		}

		void post_unpack() override
		{
			if (!game::environment::is_sp())
			{
				return;
			}

			// disable WMI and remove Hardware Query (uses WMI)
			utils::hook::set<uint8_t>(0x1400450C0, 0xC3); // WMI
			utils::hook::set<uint8_t>(0x140311790, 0xC3); // Hardware query
		}

		void* load_import(const std::string& library, const std::string& function) override
		{
			if (function == "CoInitializeEx")
			{
				return co_initialize_ex_stub;
			}

			return nullptr;
		}
	};
}

REGISTER_COMPONENT(wmi::component)
