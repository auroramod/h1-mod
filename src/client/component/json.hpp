#pragma once
#include "loader/component_loader.hpp"

#include "game/scripting/execution.hpp"

#include <json.hpp>

class json final : public component_interface
{
public:
	void post_unpack() override;

	static std::string gsc_to_string(const scripting::script_value& value);

private:
	static nlohmann::json entity_to_array(unsigned int id);
	static nlohmann::json vector_to_array(const float* value);
	static nlohmann::json gsc_to_json(scripting::script_value _value);
	static scripting::script_value json_to_gsc(nlohmann::json obj);
};
