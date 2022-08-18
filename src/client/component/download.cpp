#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "download.hpp"

namespace download
{
	namespace
	{
		
	}

	void initiate_client_download(const std::string& mod, bool has_password)
	{

	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{

		}
	};
}

REGISTER_COMPONENT(download::component)
