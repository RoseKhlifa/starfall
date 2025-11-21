#pragma once
#include "../includes.h"

namespace valhalla
{
	void render();
	void loop();
	void input();
}

namespace settings
{
	inline bool menu = false;
	inline bool teamcheck = false;

	inline bool box = false;
	inline bool fullbox = false;
	inline bool skeleton = false;
	inline bool distance = false;
	inline bool healthbar = false;
	inline bool chinahat = false;
	inline bool chams = true;
	inline bool oofarrows = false;
	inline bool tdbox = false;
	inline bool blackhole = false;
	inline bool sealesp = false;
	inline bool headdot = false;

	inline bool enabled = false;
	inline bool silent = false;
	inline bool vischeck = false;
	inline bool showfov = false;
	inline float fov = 25;
	inline float smoothing = 1.f;
	inline int key = 0x06;

	inline float espcolor[4];
	inline float iespcolor[4];
	inline float skeletoncolor[4];
	inline float iskeletoncolor[4];
	inline float espinvisdistance = 90.f;
	inline float espvisdistance = 90.f;
}
