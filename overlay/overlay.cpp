#include "overlay.h"
#include "icons.h"
#include "font.h"
#include "logo.h"

CurrentProcess Process = {};
OverlayWindow Overlay = {};
DirectX9Interface DirectX9 = {
	nullptr, nullptr, {}, {-1}, {0}
};

auto hijackwnd() -> bool
{
	 
	Overlay.Hwnd = FindWindowA(skCrypt("Chrome_WidgetWin_1"), skCrypt("Discord Overlay"));

	if (!Overlay.Hwnd)
	{
		system("cls");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
		MessageBoxA(0, skCrypt("Failed To Find Discord."), skCrypt("Overlay"), MB_OK);
		Sleep(5000);
		exit(1);
		return false;
	}

	HWND(overlaywindow);
	return true;
}

auto dx9() -> bool
{
	 
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9.IDirect3D9))) {
		return false;
	}

	D3DPRESENT_PARAMETERS Params = { 0 };
	Params.Windowed = TRUE;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.hDeviceWindow = Overlay.Hwnd;
	Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	Params.BackBufferWidth = Width;
	Params.BackBufferHeight = Height;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	if (FAILED(DirectX9.IDirect3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Overlay.Hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9.pDevice))) {
		DirectX9.IDirect3D9->Release();
		return false;
	}

	if (!DirectX9.pDevice)
		return false;


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	io.IniFilename = NULL;

	io.FontDefault = io.Fonts->AddFontFromFileTTF(skCrypt("C:/Windows/Fonts/consola.ttf"), 13.0f);
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;      
	icons_config.PixelSnapH = true;

	ImFont* funnyicons = io.Fonts->AddFontFromMemoryTTF(
		fontawesome,
		sizeof(fontawesome),
		16.0f,                          
		&icons_config,
		icons_ranges
	);

	D3DXCreateTextureFromFileInMemory(DirectX9.pDevice, (LPVOID)logos, sizeof(logos), &logo);

	D3DXCreateTextureFromFileInMemory(DirectX9.pDevice, (LPVOID)fireseal, sizeof(fireseal), &yoseal);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 5.0f;
	style.FrameRounding = 3.0f;
	style.GrabRounding = 3.0f;

	style.WindowPadding = ImVec2(8, 8);
	style.FramePadding = ImVec2(6, 4);
	style.ItemSpacing = ImVec2(6, 4);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.ScrollbarSize = 10.0f;
	style.GrabMinSize = 8.0f;

	// Compact controls
	style.FrameBorderSize = 0.5f;
	style.WindowBorderSize = 1.0f;
	style.ChildRounding = 3.0f;

	// Colors
	ImVec4* colors = style.Colors;
	// ----- Base colors -----
	ImVec4 text = ImVec4(0.8f, 0.8f, 1.0f, 1.0f);  // (204,204,255)
	ImVec4 bgDark = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);
	ImVec4 bgMid = ImVec4(0.10f, 0.10f, 0.13f, 1.0f);
	ImVec4 bgLight = ImVec4(0.16f, 0.16f, 0.20f, 1.0f);
	ImVec4 accent = ImVec4(0.35f, 0.35f, 0.45f, 1.0f);


	// ----- Text -----
	colors[ImGuiCol_Text] = text;
	colors[ImGuiCol_TextDisabled] = ImVec4(text.x, text.y, text.z, 0.45f);

	// ----- Windows -----
	colors[ImGuiCol_WindowBg] = bgDark;
	colors[ImGuiCol_ChildBg] = bgMid;
	colors[ImGuiCol_PopupBg] = bgMid;

	// ----- Borders & separators -----
	colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.3f, 0.45f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

	// ----- Frames / inputs -----
	colors[ImGuiCol_FrameBg] = bgLight;
	colors[ImGuiCol_FrameBgHovered] = ImVec4(accent.x, accent.y, accent.z, 0.75f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(accent.x, accent.y, accent.z, 1.0f);

	// ----- Title bar -----
	colors[ImGuiCol_TitleBg] = bgMid;
	colors[ImGuiCol_TitleBgActive] = bgLight;
	colors[ImGuiCol_TitleBgCollapsed] = bgDark;

	// ----- Scrollbar -----
	colors[ImGuiCol_ScrollbarBg] = bgDark;
	colors[ImGuiCol_ScrollbarGrab] = bgLight;
	colors[ImGuiCol_ScrollbarGrabHovered] = accent;
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(accent.x, accent.y, accent.z, 0.9f);

	// ----- Buttons -----
	colors[ImGuiCol_Button] = bgLight;
	colors[ImGuiCol_ButtonHovered] = accent;
	colors[ImGuiCol_ButtonActive] = ImVec4(accent.x, accent.y, accent.z, 1.0f);

	// ----- Headers (tree, selectable) -----
	colors[ImGuiCol_Header] = bgLight;
	colors[ImGuiCol_HeaderHovered] = accent;
	colors[ImGuiCol_HeaderActive] = ImVec4(accent.x, accent.y, accent.z, 1.0f);

	// ----- Tabs -----
	colors[ImGuiCol_Tab] = bgMid;
	colors[ImGuiCol_TabHovered] = accent;
	colors[ImGuiCol_TabActive] = bgLight;
	colors[ImGuiCol_TabUnfocused] = bgMid;
	colors[ImGuiCol_TabUnfocusedActive] = bgLight;

	// ----- Sliders / grab -----
	colors[ImGuiCol_SliderGrab] = accent;
	colors[ImGuiCol_SliderGrabActive] = ImVec4(accent.x, accent.y, accent.z, 1.0f);

	// ----- Resize grip -----
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.2f, 0.2f, 0.25f, 0.0f);
	colors[ImGuiCol_ResizeGripHovered] = accent;
	colors[ImGuiCol_ResizeGripActive] = ImVec4(accent.x, accent.y, accent.z, 1.0f);

	// ----- Misc -----
	colors[ImGuiCol_CheckMark] = text;
	colors[ImGuiCol_Separator] = ImVec4(0.35f, 0.35f, 0.4f, 0.5f);
	colors[ImGuiCol_SeparatorHovered] = accent;
	colors[ImGuiCol_SeparatorActive] = accent;




	ImGui_ImplWin32_Init(Overlay.Hwnd);
	ImGui_ImplDX9_Init(DirectX9.pDevice);
	DirectX9.IDirect3D9->Release();


	return true;
}

