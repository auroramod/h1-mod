#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

class fog_editor final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void cg_parse_client_visionset_triggers_stub(const char* buffer);
	static void build_atmos_fog_buffer();
	static void render_window();
};
#endif
