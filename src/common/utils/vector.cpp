#include "vector.hpp"
#include <cmath>

namespace utils::vector
{
	float normalize(float* v)
	{
		const auto length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

		if (length)
		{
			const auto ilength = 1 / length;
			v[0] *= ilength;
			v[1] *= ilength;
			v[2] *= ilength;
		}

		return length;
	}

	void copy(const float* in, float* out, int size)
	{
		for (auto i = 0; i < size; i++)
		{
			out[i] = in[i];
		}
	}

	void scale(const float* in, float scale, float* out)
	{
		out[0] = in[0] * scale;
		out[1] = in[1] * scale;
		out[2] = in[2] * scale;
	}

	void ma(const float* v1, float scale, const float* v2, float* out)
	{
		out[0] = v1[0] + scale * v2[0];
		out[1] = v1[1] + scale * v2[1];
		out[2] = v1[2] + scale * v2[2];
	}

	void subtract(const float* veca, const float* vecb, float* out)
	{
		out[0] = veca[0] - vecb[0];
		out[1] = veca[1] - vecb[1];
		out[2] = veca[2] - vecb[2];
	}

	void add(const float* veca, const float* vecb, float* out)
	{
		out[0] = veca[0] + vecb[0];
		out[1] = veca[1] + vecb[1];
		out[2] = veca[2] + vecb[2];
	}
	
	float length(float* v)
	{
		return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	}

	float product(const float* v1, const float* v2)
	{
		return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
	}
}
