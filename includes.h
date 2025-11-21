#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <string>
#include <Psapi.h>
#include <fstream>
#include <dwmapi.h>
#include <d3d9.h> 
#include <chrono>
#include <thread>
#include <winuser.h>
#include <sstream>
#include <TlHelp32.h>
#include <d3dx9.h>

#include <algorithm>  // for std::sort
#include <cmath>      // for sinf, cosf, M_PI
#include <vector> 

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"

#include <km/kernel.h>
#include "overlay/overlay.h"

#include "sdk/sdk.h"
#include "cheat/cache.h"  
#include "cheat/player.h"
#include "loop/loop.h"

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")

inline const char* username = " ";