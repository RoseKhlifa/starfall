#pragma once
#include "../includes.h"

inline int Width = GetSystemMetrics(SM_CXSCREEN);
inline int Height = GetSystemMetrics(SM_CYSCREEN);

inline IDirect3DTexture9* sillycar = nullptr;

struct CurrentProcess {
	DWORD ID;
	HANDLE Handle;
	HWND Hwnd;
	WNDPROC WndProc;
	int WindowWidth;
	int WindowHeight;
	int WindowLeft;
	int WindowRight;
	int WindowTop;
	int WindowBottom;
	LPCSTR Title;
	LPCSTR ClassName;
	LPCSTR Path;
};

extern CurrentProcess Process;


struct OverlayWindow {
	WNDCLASSEX WindowClass;
	HWND Hwnd;
	LPCSTR Name;
};

extern OverlayWindow Overlay;


struct DirectX9Interface {
	IDirect3D9Ex* IDirect3D9 = NULL;
	IDirect3DDevice9Ex* pDevice = NULL;
	D3DPRESENT_PARAMETERS pParameters = { NULL };
	MARGINS Margin = { -1 };
	MSG Message = { NULL };
};

extern unsigned char fontawesome[];
inline unsigned int fontawesome_size;
inline IDirect3DTexture9* logo = nullptr;
inline IDirect3DTexture9* yoseal = nullptr;

extern DirectX9Interface DirectX9;

extern auto hijackwnd() -> bool;
extern auto dx9() -> bool;