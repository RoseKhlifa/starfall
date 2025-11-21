#include "loop.h"
#include <overlay/icons.h>
#include <configs/configs.hpp>

static int keystatus = 0;
static int keystatus2 = 0;
namespace hotkeys
{
	int aimkey;
	int triggerkey;
}

void ChangeKey(void* blank)
{
	keystatus = 1;
	while (true)
	{
		for (int i = 0; i < 0x87; i++)
		{
			if (GetKeyState(i) & 0x8000)
			{
				settings::key = i;
				keystatus = 0;
				return;
			}
		}
	}
}

static const char* keyNames[] =
{
	(""),
	("Left Mouse"),
	("Right Mouse"),
	("Cancel"),
	("Middle Mouse"),
	("Mouse 5"),
	("Mouse 4"),
	(""),
	("Backspace"),
	("Tab"),
	(""),
	(""),
	("Clear"),
	("Enter"),
	(""),
	(""),
	("Shift"),
	("Control"),
	("Alt"),
	("Pause"),
	("Caps"),
	(""),
	(""),
	(""),
	(""),
	(""),
	(""),
	("Escape"),
	(""),
	(""),
	(""),
	(""),
	("Space"),
	("Page Up"),
	("Page Down"),
	("End"),
	("Home"),
	("Left"),
	("Up"),
	("Right"),
	("Down"),
	(""),
	(""),
	(""),
	("Print"),
	("Insert"),
	("Delete"),
	(""),
	("0"),
	("1"),
	("2"),
	("3"),
	("4"),
	("5"),
	("6"),
	("7"),
	("8"),
	("9"),
	(""),
	(""),
	(""),
	(""),
	(""),
	(""),
	(""),
	("A"),
	("B"),
	("C"),
	("D"),
	("E"),
	("F"),
	("G"),
	("H"),
	("I"),
	("J"),
	("K"),
	("L"),
	("M"),
	("N"),
	("O"),
	("P"),
	("Q"),
	("R"),
	("S"),
	("T"),
	("U"),
	("V"),
	("W"),
	("X"),
	("Y"),
	("Z"),
	(""),
	(""),
	(""),
	(""),
	(""),
	("Numpad 0"),
	("Numpad 1"),
	("Numpad 2"),
	("Numpad 3"),
	("Numpad 4"),
	("Numpad 5"),
	("Numpad 6"),
	("Numpad 7"),
	("Numpad 8"),
	("Numpad 9"),
	("Multiply"),
	("Add"),
	(""),
	("Subtract"),
	("Decimal"),
	("Divide"),
	("F1"),
	("F2"),
	("F3"),
	("F4"),
	("F5"),
	("F6"),
	("F7"),
	("F8"),
	("F9"),
	("F10"),
	("F11"),
	("F12")
};
static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = (const char* const*)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}
void HotkeyButton(int aimkey, void* changekey, int status)
{
	const char* preview_value = NULL;
	if (aimkey >= 0 && aimkey < IM_ARRAYSIZE(keyNames))
		Items_ArrayGetter(keyNames, aimkey, &preview_value);

	std::string aimkeys;
	if (preview_value == NULL)
		aimkeys = skCrypt("Select Key").decrypt();
	else
		aimkeys = preview_value;

	if (status == 1)
	{
		aimkeys = skCrypt("Press the key").decrypt();
	}
	if (ImGui::Button(aimkeys.c_str(), ImVec2(125, 25)))
	{
		if (status == 0)
		{
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekey, nullptr, 0, nullptr);
		}
	}
}

void ToggleSwitch(const char* label, bool* v)
{
	ImGui::PushID(label); // Ensure unique ID

	// Display the label first
	ImGui::Text("%s", label);
	ImGui::SameLine(150); // Adjust spacing from the label to the switch

	// Switch dimensions
	ImVec2 p = ImGui::GetCursorScreenPos();
	float height = ImGui::GetFrameHeight();
	float width = height * 1.5f;

	ImGui::InvisibleButton("switch", ImVec2(width, height));
	if (ImGui::IsItemClicked())
		*v = !*v;

	ImU32 col_bg = *v ? IM_COL32(149, 194, 194, 255) : IM_COL32(28, 32, 32, 255);
	ImU32 col_circle = IM_COL32(56, 52, 52, 255);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
	draw_list->AddCircleFilled(ImVec2(p.x + (*v ? width - height / 2 : height / 2), p.y + height / 2), height / 2 - 2, col_circle);

	ImGui::PopID();
}

void DrawValueSlider(const char* label, float* value, float min, float max, const ImVec2& size = ImVec2(0, 20))
{
	ImGui::PushID(label);

	// Display label
	ImGui::Text("%s", label);
	ImGui::SameLine();

	// Draw slider manually
	ImVec2 p = ImGui::GetCursorScreenPos();
	float width = size.x > 0 ? size.x : ImGui::GetContentRegionAvail().x - 50; // leave space for value
	float height = size.y;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	// Colors
	ImU32 col_bg = IM_COL32(28, 32, 32, 255);       // background track
	ImU32 col_fill = IM_COL32(149, 194, 194, 255);    // filled portion
	ImU32 col_circle = IM_COL32(56, 52, 52, 255);       // knob circle

	// Slider background
	draw_list->AddRectFilled(
		p,
		ImVec2(p.x + width, p.y + height),
		col_bg,
		height * 0.5f
	);

	// Slider filled portion
	float t = (*value - min) / (max - min);
	float filled = width * t;
	draw_list->AddRectFilled(
		p,
		ImVec2(p.x + filled, p.y + height),
		col_fill,
		height * 0.5f
	);

	// Circle knob
	float knob_x = p.x + filled;
	float knob_y = p.y + height * 0.5f;
	float knob_radius = height * 0.6f; // knob slightly bigger than bar height
	draw_list->AddCircleFilled(ImVec2(knob_x, knob_y), knob_radius, col_circle, 32);

	// Handle input
	ImGui::InvisibleButton("slider", ImVec2(width, height));
	if (ImGui::IsItemActive() && ImGui::GetIO().MouseDown[0])
	{
		float mouse_x = ImGui::GetIO().MousePos.x - p.x;
		t = ImClamp(mouse_x / width, 0.0f, 1.0f);
		*value = min + t * (max - min);
	}

	// Display numeric value at the right
	ImGui::SameLine();
	ImGui::Text("%d", static_cast<int>(*value));

	ImGui::Dummy(ImVec2(0, height + 4)); // spacing
	ImGui::PopID();
}

namespace valhalla
{
	void input()
	{
		ImGuiIO& io = ImGui::GetIO();

		// -----------------------------
		// Mouse Input
		// -----------------------------
		for (int i = 0; i < 5; i++)
			io.MouseDown[i] = false;

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			io.MouseDown[0] = true;

		// -----------------------------
		// Keyboard Input
		// -----------------------------
		for (int key = 0; key < 512; key++)
			io.KeysDown[key] = (GetAsyncKeyState(key) & 0x8000);

		io.KeyCtrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000);
		io.KeyShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
		io.KeyAlt = (GetAsyncKeyState(VK_MENU) & 0x8000);

		// -----------------------------
		// Character Input (for typing)
		// -----------------------------
		// Use GetAsyncKeyState to detect typed keys
		for (int key = 0x30; key <= 0x5A; key++) // 0-9 and A-Z
		{
			if (GetAsyncKeyState(key) & 1) // 1 = newly pressed (transition)
			{
				BYTE keyboardState[256];
				GetKeyboardState(keyboardState);

				WCHAR unicodeChar[4];
				if (ToUnicode(key, MapVirtualKey(key, MAPVK_VK_TO_VSC), keyboardState, unicodeChar, 4, 0) > 0)
					io.AddInputCharacter(unicodeChar[0]);
			}
		}

		// You can also add special characters if needed
	}

	int tab;
	const char* tabname = skCrypt("starfall").decrypt();
	const ImVec2& size = ImVec2(0, 6);
	void drawmenu()
	{
		ImGui::SetNextWindowSize(ImVec2(600, 400));
		if (GetAsyncKeyState(VK_INSERT) & 1) settings::menu = !settings::menu;

		if (!settings::menu) return;

		ImGui::Begin(skCrypt("starfall"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		{

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 50.f);

			float columnWidth = ImGui::GetColumnWidth();
			float logoWidth = 40.f; // logo size

			// Center the logo
			ImGui::Image((void*)logo, ImVec2(logoWidth, logoWidth));

			auto buttonSize = ImVec2(40, 40);


			if (ImGui::Button(skCrypt("aim"), buttonSize))
				tab = 1;

			if (ImGui::Button(skCrypt("esp"), buttonSize))
				tab = 2;

			if (ImGui::Button(skCrypt("col"), buttonSize))
				tab = 3;

			if (ImGui::Button(skCrypt("cfg"), buttonSize))
				tab = 4;

			ImGui::NextColumn();
			{
				if (tab == 1)
				{
					float totalWidth = ImGui::GetContentRegionAvail().x;
					float spacing = 25.0f;
					float childWidth = (totalWidth - spacing) / 2.0f;

					ImGui::Spacing();

					ImGui::BeginChild("AimbotLeft", ImVec2(childWidth, 250), true);
					ToggleSwitch(skCrypt("enable"), &settings::enabled);
					ToggleSwitch(skCrypt("silent"), &settings::silent);
					ToggleSwitch(skCrypt("visible check"), &settings::vischeck);
					HotkeyButton(settings::key, ChangeKey, keystatus);
					ImGui::EndChild();

					ImGui::SameLine(); 

					ImGui::BeginChild("AimbotRight", ImVec2(childWidth, 110), true);
					ToggleSwitch(skCrypt("show fov"), &settings::showfov);
					DrawValueSlider(skCrypt("fov"), &settings::fov, 1.f, 1800.f, size);
					DrawValueSlider(skCrypt("smoothing"), &settings::smoothing, 1.f, 30.f, size);
					ImGui::EndChild();
				}

				if (tab == 2)
				{
					float totalWidth = ImGui::GetContentRegionAvail().x;
					float spacing = 25.0f;
					float childWidth = (totalWidth - spacing) / 2.0f;

					ImGui::Spacing();

					ImGui::BeginChild("ESPLeft", ImVec2(childWidth, 325), true);
					ToggleSwitch(skCrypt("corner box"), &settings::box);
					ToggleSwitch(skCrypt("normal box"), &settings::fullbox);
					ToggleSwitch(skCrypt("seal esp"), &settings::sealesp);
					ToggleSwitch(skCrypt("3d box"), &settings::tdbox);
					ToggleSwitch(skCrypt("china hat"), &settings::chinahat);
					ToggleSwitch(skCrypt("black hole"), &settings::blackhole);
					ToggleSwitch(skCrypt("health bar"), &settings::healthbar);
					ToggleSwitch(skCrypt("distance"), &settings::distance);
					ToggleSwitch(skCrypt("skeleton"), &settings::skeleton);
					ToggleSwitch(skCrypt("head dot"), &settings::headdot);
					ToggleSwitch(skCrypt("oof arrows"), &settings::oofarrows);
					ImGui::EndChild();

					ImGui::SameLine();

					ImGui::BeginChild("ESPRight", ImVec2(childWidth, 110), true);
					DrawValueSlider(skCrypt("visible distance"), &settings::espvisdistance, 1.f, 1000.f, size);
					DrawValueSlider(skCrypt("invisible distance"), &settings::espinvisdistance, 1.f, 1000.f, size);

					ImGui::EndChild();
				}

				if (tab == 3)
				{
					float totalWidth = ImGui::GetContentRegionAvail().x;
					float spacing = 25.0f;
					float childWidth = (totalWidth - spacing) / 2.0f;

					ImGui::Spacing();

					// LEFT CHILD
					ImGui::BeginChild("ColorsLeft", ImVec2(childWidth, 110), true);
					ImGui::ColorEdit3(skCrypt("esp color"), settings::espcolor);
					ImGui::ColorEdit3(skCrypt("invisible esp color"), settings::iespcolor);
					if (ImGui::Button(skCrypt("exit")))
					{
						exit(1);
					}
					ImGui::EndChild();

					ImGui::SameLine();

					// RIGHT CHILD
					ImGui::BeginChild("ColorsRight", ImVec2(childWidth, 110), true);
					ImGui::ColorEdit3(skCrypt("skeleton color"), settings::skeletoncolor);
					ImGui::ColorEdit3(skCrypt("invisible skeleton color"), settings::iskeletoncolor);
					ImGui::EndChild();


				}

				if (tab == 4)
				{
					config::render_menu();
				}
			}
		}
		ImGui::End();
	}

	void DrawRhombus(ImDrawList* draw_list, ImVec2 center, float width, float height, ImU32 fillColor, ImU32 outlineColor, float thickness = 2.0f)
	{
		ImVec2 top = ImVec2(center.x, center.y - height / 2.0f);
		ImVec2 right = ImVec2(center.x + width / 2.0f, center.y);
		ImVec2 bottom = ImVec2(center.x, center.y + height / 2.0f);
		ImVec2 left = ImVec2(center.x - width / 2.0f, center.y);

		ImVec2 points[4] = { top, right, bottom, left };
		draw_list->AddConvexPolyFilled(points, 4, fillColor);
		draw_list->AddPolyline(points, 4, outlineColor, true, thickness);
	}

	// Draw watermark
	void DrawWatermark(const char* name, const char* keyExpiry)
	{
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		ImVec2 screenSize = ImGui::GetIO().DisplaySize;


		ImVec2 pos = ImVec2(20, 20);

		
		ImVec2 rhombusCenter = ImVec2(pos.x + 15, pos.y + 15);
		DrawRhombus(draw_list, rhombusCenter, 20, 20,
			IM_COL32(204, 204, 255, 200),
			IM_COL32(255, 255, 255, 255), 2.0f);


		ImVec2 textPos = ImVec2(pos.x + 40, pos.y);
		draw_list->AddText(textPos, IM_COL32(204, 204, 255, 255), name);


		ImVec2 expiryPos = ImVec2(pos.x + 40, pos.y + 15);
		std::string expiryText = std::string("Username: ") + keyExpiry;
		draw_list->AddText(expiryPos, IM_COL32(180, 180, 180, 255), expiryText.c_str());

			float fps = ImGui::GetIO().Framerate;
			ImVec2 fpsPos = ImVec2(pos.x + 40, pos.y + 30); // 15px below expiry line
			char fpsText[32];
			snprintf(fpsText, sizeof(fpsText), "FPS: %.0f", fps);
			draw_list->AddText(fpsPos, IM_COL32(180, 180, 180, 255), fpsText);
	}


	void render()
	{
		if (GetAsyncKeyState(VK_INSERT) & 1)
			settings::menu = !settings::menu;
		input();
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		players::hack_loop();


		if (settings::showfov)
		{
			ImGui::GetForegroundDrawList()->AddCircle(ImVec2(Width / 2, Height / 2), settings::fov, ImColor(204, 204, 255), 64, 0);
		}

		ImGuiIO& io = ImGui::GetIO();
		ImGuiViewport* vp = ImGui::GetMainViewport();
		ImDrawList* dl = ImGui::GetForegroundDrawList();

		const char* text = skCrypt("starfall").decrypt();
		const ImFont* font = io.FontDefault;

		float font_size = vp->Size.x * 0.05f; 

		
		ImVec2 txtSize = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text);

	
		ImVec2 pos(
			vp->Pos.x + (vp->Size.x - txtSize.x) * 0.5f,
			vp->Pos.y + (vp->Size.y - txtSize.y) * 0.5f
		);


		ImU32 col = ImGui::GetColorU32(ImVec4(1, 1, 1, 0.12f));

		dl->AddText(font, font_size, pos, col, text);

		DrawWatermark("starfall | by luk", username);

		ImGui::GetIO().MouseDrawCursor = settings::menu;

		if (settings::menu) {
			drawmenu();

			SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
			UpdateWindow(Overlay.Hwnd);
			SetFocus(Overlay.Hwnd);

		}
		else {
			SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
			SetFocus(nullptr);
		}

		ImGui::EndFrame();

		if (DirectX9.pDevice) {
			DirectX9.pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
			if (DirectX9.pDevice->BeginScene() >= 0) {
				ImGui::Render();
				ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
				DirectX9.pDevice->EndScene();
			}

			HRESULT result = DirectX9.pDevice->Present(NULL, NULL, NULL, NULL);
			if (result == D3DERR_DEVICELOST && DirectX9.pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
				ImGui_ImplDX9_InvalidateDeviceObjects();
				DirectX9.pDevice->Reset(&DirectX9.pParameters);
				ImGui_ImplDX9_CreateDeviceObjects();
			}
		}
	}


	void loop()
	{
		static RECT oldRect = { 0 };
		ZeroMemory(&DirectX9.Message, sizeof(MSG));


		while (DirectX9.Message.message != WM_QUIT)
		{
			if (PeekMessage(&DirectX9.Message, Overlay.Hwnd, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&DirectX9.Message);
				DispatchMessage(&DirectX9.Message);
			}
			render();
		}
	}
}