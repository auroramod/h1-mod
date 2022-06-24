#pragma once

namespace utils::vector
{
	float normalize(float* v);
	void copy(const float* in, float* out, int size = 3);
	void scale(const float* in, float scale, float* out);
	void ma(const float* v1, float scale, const float* v2, float* out);
	void subtract(const float* veca, const float* vecb, float* out);
	float length(float* v);
	float product(const float* v1, const float* v2);
}
