#include "main.hpp"

int Simulation::Width = 0, Simulation::Height = 0;

int __stdcall wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
    HWND hWnd = Simulation::Main::InitializeWindow(hInstance);
    Simulation::Graphics graphics = Simulation::Main::InitializeGraphics(hWnd);
    if (graphics.HasSucceededLastOperation()) {
        Simulation::Main::StartUpdating();
        Simulation::Main::MessageLoop();
    }
    return 0;
}

HWND Simulation::Main::InitializeWindow(HINSTANCE hInstance) {
    // Create the window class
    WNDCLASS wc = { 0 };
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Simulation";
    // Set black background
    wc.hbrBackground = CreateSolidBrush(0x000000);
    // Set the event handler
    wc.lpfnWndProc = WindowProc;
    // Register the window class
    RegisterClass(&wc);
    // Create the window
    HWND hWnd = CreateWindow(wc.lpszClassName, nullptr, WS_VISIBLE | WS_MAXIMIZE | WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);
    // Save the screen size
    RECT rect;
    GetWindowRect(GetDesktopWindow(), &rect);
    Width = rect.right;
    Height = rect.bottom;
    // Hide the cursor
    ShowCursor(false);
    // Return the created window
    return hWnd;
}

Simulation::Graphics Simulation::Main::InitializeGraphics(HWND hWnd) {
    return Graphics(hWnd);
}

void Simulation::Main::StartUpdating() {

}

void Simulation::Main::MessageLoop() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}