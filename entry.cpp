#include "includes.h"
#include <km/kernel.h>
#include <km/utils.h>
#include <ctime>
#include <configs/configs.hpp>
#ifdef PDB_OFFSETS
#include "KDSymbolsHandler.h"
#endif
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

std::string GetWindowsUsername() {
    char username[256];
    DWORD size = sizeof(username);
    if (GetUserNameA(username, &size)) {
        return std::string(username);
    }
    return skCrypt("Unknown").decrypt();
}
using namespace std;

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4, // Windows 10 1803+
    ACCENT_ENABLE_HOSTBACKDROP = 5,       // Windows 11+
    ACCENT_INVALID_STATE = 6
};

struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;     // e.g., 2 = Draw border
    DWORD GradientColor;   // ARGB
    DWORD AnimationId;
};

enum WINDOWCOMPOSITIONATTRIB {
    WCA_UNDEFINED = 0,
    WCA_NCRENDERING_ENABLED = 1,
    WCA_NCRENDERING_POLICY = 2,
    WCA_TRANSITIONS_FORCEDISABLED = 3,
    WCA_ALLOW_NCPAINT = 4,
    WCA_CAPTION_BUTTON_BOUNDS = 5,
    WCA_NONCLIENT_RTL_LAYOUT = 6,
    WCA_FORCE_ICONIC_REPRESENTATION = 7,
    WCA_EXTENDED_FRAME_BOUNDS = 8,
    WCA_HAS_ICONIC_BITMAP = 9,
    WCA_THEME_ATTRIBUTES = 10,
    WCA_NCRENDERING_EXILED = 11,
    WCA_NCADORNMENTINFO = 12,
    WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
    WCA_VIDEO_OVERLAY_ACTIVE = 14,
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
    WCA_DISALLOW_PEEK = 16,
    WCA_CLOAK = 17,
    WCA_CLOAKED = 18,
    WCA_ACCENT_POLICY = 19,
    WCA_FREEZE_REPRESENTATION = 20,
    WCA_EVER_UNCLOAKED = 21,
    WCA_VISUAL_OWNER = 22,
    WCA_LAST = 23
};

struct WINCOMPATTRDATA {
    WINDOWCOMPOSITIONATTRIB Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
};

void EnableBlur(HWND hwnd) {
    const HINSTANCE hModule = LoadLibrary(TEXT("user32.dll"));
    if (hModule) {
        typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
        auto SetWindowCompositionAttribute =
            (pSetWindowCompositionAttribute)GetProcAddress(hModule, "SetWindowCompositionAttribute");

        if (SetWindowCompositionAttribute) {
            ACCENT_POLICY accent = { ACCENT_ENABLE_BLURBEHIND, 0, 0, 0 };
            WINCOMPATTRDATA data = { WCA_ACCENT_POLICY, &accent, sizeof(accent) };
            SetWindowCompositionAttribute(hwnd, &data);
        }
        FreeLibrary(hModule);
    }
}

void log(std::string s)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* local_tm = std::localtime(&now_time);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD defaultColor = csbi.wAttributes;

    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "[";

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << std::setfill('0')
        << std::setw(2) << local_tm->tm_hour << ":"
        << std::setw(2) << local_tm->tm_min << ":"
        << std::setw(2) << local_tm->tm_sec;

    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "] ";

    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "[";
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << "~";
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "] | ";

    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << s << std::endl;
}

void log_error(std::string s)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // get current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* local_tm = std::localtime(&now_time);

    // save original console color
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD defaultColor = csbi.wAttributes;

    // print timestamp in red
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "[";

    // reset to white for time
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << std::setfill('0')
        << std::setw(2) << local_tm->tm_hour << ":"
        << std::setw(2) << local_tm->tm_min << ":"
        << std::setw(2) << local_tm->tm_sec;

    // back to red for closing bracket
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "] ";

    // print ~ in white with red brackets
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "[";
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << "!";
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "] | ";

    // reset to default color for the message
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << s << std::endl;
}

bool setConsoleSize(int width, int height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return false;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    // Step 1: Shrink window to minimum so buffer can shrink
    SMALL_RECT minRect = { 0, 0, 1, 1 };
    SetConsoleWindowInfo(hConsole, TRUE, &minRect);

    // Step 2: Set new buffer size
    COORD newSize;
    newSize.X = width;
    newSize.Y = height;
    if (!SetConsoleScreenBufferSize(hConsole, newSize)) {
        std::cerr << "Buffer resize failed: " << GetLastError() << "\n";
        return false;
    }

    // Step 3: Expand window to requested size
    SMALL_RECT rect;
    rect.Left = 0;
    rect.Top = 0;
    rect.Right = width - 1;
    rect.Bottom = height - 1;
    if (!SetConsoleWindowInfo(hConsole, TRUE, &rect)) {
        std::cerr << "Window resize failed: " << GetLastError() << "\n";
        return false;
    }

    return true;
}

void removeScrollBar() {
    HWND console = GetConsoleWindow();
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(hOut, &csbi);

    // Set the buffer size to match the window size
    COORD newSize;
    newSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    newSize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    SetConsoleScreenBufferSize(hOut, newSize);
}

void hideTitleBar() {
    HWND hwnd = GetConsoleWindow();
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME); // remove title bar and border
    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED);
}

void setupConsole(int width, int height) {
    HWND hwnd = GetConsoleWindow();
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // 1. Remove title bar and border
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME);
    SetWindowLong(hwnd, GWL_STYLE, style);

    // 2. Set console buffer size to match window size (removes scroll bars)
    COORD bufferSize = { (SHORT)width, (SHORT)height };
    SetConsoleScreenBufferSize(hOut, bufferSize);

    SMALL_RECT windowSize = { 0, 0, (SHORT)(width - 1), (SHORT)(height - 1) };
    SetConsoleWindowInfo(hOut, TRUE, &windowSize);

    // 3. Center the console on screen
    RECT screenRect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);

    int screenWidth = screenRect.right - screenRect.left;
    int screenHeight = screenRect.bottom - screenRect.top;

    int posX = (screenWidth - width * 8) / 2;   // Approximate width of char = 8 px
    int posY = (screenHeight - height * 16) / 2; // Approximate height of char = 16 px

    SetWindowPos(hwnd, NULL, posX, posY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

bool done = false;


void enableDragging(HWND hwnd) {
    ReleaseCapture();
    SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

const int DRAG_HEIGHT = 20;

void updateblur()
{
    HWND hWnd = GetConsoleWindow();
    bool dragging = false;
    POINT clickOffset = { 0, 0 };

    while (!done)
    {
        EnableBlur(hWnd);

        SHORT keyState = GetAsyncKeyState(VK_LBUTTON);
        bool leftDown = (keyState & 0x8000) != 0;

        if (!dragging && leftDown)
        {
            // check if mouse is inside the top region
            POINT cursor;
            GetCursorPos(&cursor);

            POINT client = cursor;
            if (ScreenToClient(hWnd, &client)) {
                if (client.y >= 0 && client.y < DRAG_HEIGHT) {
                    // start dragging: store cursor -> window offset
                    RECT rc;
                    GetWindowRect(hWnd, &rc);
                    clickOffset.x = cursor.x - rc.left;
                    clickOffset.y = cursor.y - rc.top;
                    dragging = true;
                }
            }
        }
        else if (dragging)
        {
            if (leftDown)
            {
                // while dragging, move window to keep the initial offset
                POINT cursor;
                GetCursorPos(&cursor);
                int newX = cursor.x - clickOffset.x;
                int newY = cursor.y - clickOffset.y;
                SetWindowPos(hWnd, NULL, newX, newY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
            }
            else
            {
                // mouse released -> stop dragging
                dragging = false;
            }
        }
    }
}


int main()
{
    bool auth = false;
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_SHOW);
    AllocConsole();
    CreateThread(0, 0, LPTHREAD_START_ROUTINE(updateblur), 0, 0, 0);
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 15;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(hOut, FALSE, &cfi);    SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    removeScrollBar();
    hideTitleBar();
    setupConsole(80, 25);
    if (setConsoleSize(80, 25)) {
        std::cout << "Resized console to 100x30.\n";
    }
    else {
        std::cerr << "Resize failed.\n";
    }

start:
    system("cls");

    cout << reinterpret_cast <const char*>(u8R"(    ___       ___       ___       ___   
   /\  \     /\  \     /\  \     /\  \  
  /::\  \    \:\  \   /::\  \   /::\  \ 
 /\:\:\__\   /::\__\ /::\:\__\ /::\:\__\
 \:\:\/__/  /:/\/__/ \/\::/  / \;:::/  /
  \::/  /   \/__/      /:/  /   |:\/__/ 
   \/__/               \/__/     \|__| )");

    cout << reinterpret_cast <const char*>(u8R"(


╭──(starfall@valoant)——[~/v4] 
╰─────$ )");

    std::string cmd;
    std::cin >> cmd;

    if (cmd == "exit")
        exit(1);
    else if (cmd == "load")
    {
        MessageBoxA(0, skCrypt("In Lobby press F1"), "starfall", 0);
        while (!GetAsyncKeyState(VK_F1));

        km::process_id = km::find_process(skCrypt("VALORANT-Win64-Shipping.exe"));
        if (!km::process_id)
        {
            log_error("game not found");
            goto start;
        }

        km::attach_to_process(km::process_id);


        while (!km::vgk)
        {
            km::vgk = utils::GetDriverModuleBase(skCrypt("vgk.sys"));
            Sleep(1000);
        }


        log("Tab into VALORANT");

        bool WindowFocus = false;
        while (WindowFocus == false) {
            DWORD ForegroundWindowProcessID;
            GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
            if (km::find_process("VALORANT-Win64-Shipping.exe") == ForegroundWindowProcessID) {
                Process.ID = GetCurrentProcessId();
                Process.Handle = GetCurrentProcess();
                Process.Hwnd = GetForegroundWindow();

                RECT TempRect;
                GetWindowRect(Process.Hwnd, &TempRect);
                Width = TempRect.right - TempRect.left;
                Height = TempRect.bottom - TempRect.top;
                Process.WindowLeft = TempRect.left;
                Process.WindowRight = TempRect.right;
                Process.WindowTop = TempRect.top;
                Process.WindowBottom = TempRect.bottom;

                char TempTitle[MAX_PATH];
                GetWindowText(Process.Hwnd, TempTitle, sizeof(TempTitle));
                Process.Title = TempTitle;

                char TempClassName[MAX_PATH];
                GetClassName(Process.Hwnd, TempClassName, sizeof(TempClassName));
                Process.ClassName = TempClassName;

                char TempPath[MAX_PATH];
                GetModuleFileNameEx(Process.Handle, NULL, TempPath, sizeof(TempPath));
                Process.Path = TempPath;

                WindowFocus = true;
            }
        }

        if (!hijackwnd())
        {
            log_error("Failed to hijack discord overlay (new overlay actually on?)");
            goto start;
        }

        if (!dx9())
        {
            log_error("Failed to start DX9");
            goto start;
        }

        std::thread(valhalla::cache::cache).detach();
        std::thread(valhalla::players::player_loop).detach();
        //std::thread(valcheck).detach();

        config::init();
        config::refresh_list();

        log("Loaded Enjoy!");

        HWND consoleWindow = GetConsoleWindow();
        ShowWindow(consoleWindow, SW_HIDE);
        FreeConsole();

        done = true;

        Sleep(1000);

        while (true)
        {
            valhalla::loop();
        }
    }
    else
    {
        log_error(cmd + " Is not a valid command");
        Sleep(1000);
        goto start;
    }
    goto start;
}
