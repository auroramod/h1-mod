#pragma once
#include "loader/component_loader.hpp"

#ifdef _DEBUG
class experimental final : public component_interface
{
public:
	void post_unpack() override;

private:
	static float distance_2d(float*, float*);
	static float distance_3d(float*, float*);
	static void crossProduct3D(float[3], float[3], float[3]);
	static void normalize3D(float[3]);
	static void calculateTriangleNormal(float[3][3], float[3]);
	static float dotProduct3D(float[3], float[3]);
	static float distancePointToPlane(float[3], float[3], float[3]);
	static void calculateBarycentricCoordinates(float[3][3], float[3], float&, float&, float&);
	static bool lineTriangleIntersection(float[3][3], float[3], float[3]);
	static void getCenterPoint(float[3][3], float[3]);
	static void render_draw_material();
	static void cg_draw2d_stub(int);
};

#endif
