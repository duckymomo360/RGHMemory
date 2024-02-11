#pragma once
#include <glm/glm.hpp>

#define CAMMODE_REGULAR  0
#define CAMMODE_DETACHED 1
#define CAMMODE_FREECAM  2

struct Camera
{
	static int   Mode;
	static float Fov;
	static float NearPlane;
	static float FarPlane;

	static void  CreateHooks();
	static void  Draw();
	static void  Update();
};