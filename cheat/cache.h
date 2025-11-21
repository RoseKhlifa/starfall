#pragma once
#include <sdk/sdk.h>

namespace valhalla::cache
{
	inline valhalla::sdk::cache* ccache = new valhalla::sdk::cache();
	inline  valhalla::sdk::camera* ccam = new valhalla::sdk::camera();

	extern void start_uworld();
	extern void cache();
}