#include "../include/main.h"
#include "../include/event_handler.h"
#include "../include/graphics.h"
#include "../include/module_earth.h"
#include "../include/module_satellite.h"

#include <thread>

int __stdcall wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
    Simulation::Main main(hInstance);
    return main.ExitCode;
}

namespace Simulation {
    int Width, Height;
    bool Running;

    Main::Main(HINSTANCE hInstance) : hInstance(NULL), hWnd(NULL), ExitCode(0) {
        this->hInstance = hInstance;
        InitializeWindow();
        Graphics graphics(hWnd);
        if (graphics.Success()) {
            InitializeModules(graphics.GetEarthBitmap(), graphics.GetSatelliteBitmap());
            Running = true;
            StartTicking(graphics);
            MessageLoop();
        }
        else {
            ExitCode = 1;
        }
    }

    void Main::InitializeWindow() {
        WNDCLASS wc = { NULL };
        wc.hInstance = hInstance;
        wc.lpszClassName = L"Simulation";
        wc.hbrBackground = CreateSolidBrush(0x000000); // Black background
        wc.lpfnWndProc = EventHandler::WindowProc;
        RegisterClass(&wc);
        hWnd = CreateWindow(wc.lpszClassName, NULL, WS_VISIBLE | WS_MAXIMIZE | WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

        RECT rect;
        GetWindowRect(GetDesktopWindow(), &rect); // Save the screen size
        Width = rect.right;
        Height = rect.bottom;
        
        ShowCursor(false); // Hide the cursor
    }

    void Main::InitializeModules(const ID2D1Bitmap* const bmpEarth, const ID2D1Bitmap* const bmpSatellite) const {
        Earth::Initialize(bmpEarth);
        Satellite::Initialize(bmpSatellite);
    }
    
    void Main::StartTicking(Graphics& graphics) const {
        std::thread ticker(Tick, std::ref(graphics), hWnd);
        ticker.detach();
    }

    void Main::Tick(Graphics& graphics, HWND hWnd) {
        while (Running) {
            Satellite::Update();
            graphics.Draw();
            std::this_thread::sleep_for(TickDelay);
        }
        SendMessage(hWnd, WM_CLOSE, NULL, NULL);
    }

    void Main::MessageLoop() const {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}