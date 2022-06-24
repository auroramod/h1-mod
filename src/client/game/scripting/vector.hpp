#pragma once
#include "game/game.hpp"

namespace scripting
{
	class vector final
	{
	public:
		vector() = default;
		vector(const float* value);
		vector(const game::vec3_t& value);
		vector(float x, float y, float z);

		operator game::vec3_t&();
		operator const game::vec3_t&() const;

		game::vec_t& operator[](size_t i);
		const game::vec_t& operator[](size_t i) const;

		float get_x() const;
		float get_y() const;
		float get_z() const;

		void set_x(float value);
		void set_y(float value);
		void set_z(float value);

	private:
		game::vec3_t value_{0};
	};
}
