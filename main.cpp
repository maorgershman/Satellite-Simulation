#include "main.hpp"
#include "graphics.hpp"
#include "event_handler.hpp"
#include "earth.hpp"
#include "satellite.hpp"
#include "resource.hpp"
#include <thread>

int Simulation::Width = 0;
int Simulation::Height = 0;
bool Simulation::Running = true;

int __stdcall wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
    HWND hWnd = Simulation::Main::InitializeWindow(hInstance);
    Simulation::Graphics graphics = Simulation::Main::InitializeGraphics(hWnd);
    if (graphics.HasSucceededLastOperation()) {
        Simulation::Main::InitializeModules(graphics);
        Simulation::Main::StartUpdating(graphics);
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

void Simulation::Main::InitializeModules(Graphics& graphics) {
    Earth::Initialize(graphics.GetEarthBitmap());
    Satellite::Initialize(graphics.GetSatelliteBitmap());
}

void Simulation::Main::StartUpdating(Graphics& graphics) {
    std::thread updater(Tick, std::ref(graphics));
    updater.detach();
}

void Simulation::Main::Tick(Graphics& graphics) {
    while (Running) {
        Satellite::Update();
        graphics.Paint();
        std::this_thread::sleep_for(std::chrono::microseconds((int)(1000000.0 / UpdatesPerSecond)));
    }
}

void Simulation::Main::MessageLoop() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}