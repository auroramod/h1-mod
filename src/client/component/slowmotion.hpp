#pragma once
#include "loader/component_loader.hpp"

#include "game/scripting/script_value.hpp"

class slowmotion final : public component_interface
{
public:
	void post_unpack() override;

private:
	template <typename T, typename T2>
	static T get_timescale_safe(const scripting::value_wrap& arg);
};
