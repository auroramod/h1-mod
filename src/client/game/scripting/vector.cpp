#include <std_include.hpp>
#include "vector.hpp"

namespace scripting
{
	vector::vector(const float* value)
	{
		for (auto i = 0; i < 3; ++i)
		{
			this->value_[i] = value[i];
		}
	}

	vector::vector(const game::vec3_t& value)
		: vector(&value[0])
	{
	}

	vector::vector(const float x, const float y, const float z)
	{
		this->value_[0] = x;
		this->value_[1] = y;
		this->value_[2] = z;
	}

	vector::operator game::vec3_t&()
	{
		return this->value_;
	}

	vector::operator const game::vec3_t&() const
	{
		return this->value_;
	}

	game::vec_t& vector::operator[](const size_t i)
	{
		if (i >= 3)
		{
			throw std::runtime_error("Out of bounds.");
		}

		return this->value_[i];
	}

	const game::vec_t& vector::operator[](const size_t i) const
	{
		if (i >= 3)
		{
			throw std::runtime_error("Out of bounds.");
		}

		return this->value_[i];
	}

	float vector::get_x() const
	{
		return this->operator[](0);
	}

	float vector::get_y() const
	{
		return this->operator[](1);
	}

	float vector::get_z() const
	{
		return this->operator[](2);
	}

	void vector::set_x(const float value)
	{
		this->operator[](0) = value;
	}

	void vector::set_y(const float value)
	{
		this->operator[](1) = value;
	}

	void vector::set_z(const float value)
	{
		this->operator[](2) = value;
	}
}
