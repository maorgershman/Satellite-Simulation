#include "../include/main.h"
#include "../include/event_handler.h"
#include "../include/graphics.h"
#include "../include/module_earth.h"
#include "../include/module_satellite.h"
#include <thread>

namespace Simulation {
    int Width, Height;
    bool Running;
    Main MainInstance;
    Graphics* GraphicsInstance;

    int Main::Run(HINSTANCE hInstance) {
        // Create the window
        InitializeWindow(hInstance);

        // Create the graphics instance, scope is critical because of the destructor!
        Graphics graphics(hWnd);
        GraphicsInstance = &graphics;
        
        if (graphics.Success()) {
            Running = true;

            // Set the earth and satellite's initial size and position
            InitializeModules();

            // Start to update and repaint
            StartTicking();

            // Listen to events and keep the main thread alive.
            MessageLoop();
            
            // Success
            return 0;
        }

        // Failure
        return 1;
    }

    void Main::InitializeWindow(HINSTANCE hInstance) {
        // Create the window class
        WNDCLASS wc = { NULL };
        wc.hInstance = hInstance;
        wc.lpszClassName = L"Simulation";
        wc.hbrBackground = CreateSolidBrush(0x000000); // Black background
        wc.lpfnWndProc = EventHandler::WindowProc;     // Register the event handler
        RegisterClass(&wc);

        // Create the actual window, full screen
        hWnd = CreateWindow(wc.lpszClassName, NULL, WS_VISIBLE | WS_MAXIMIZE | WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

        // Save the screen size
        RECT rect;
        GetWindowRect(GetDesktopWindow(), &rect);
        Width = rect.right;
        Height = rect.bottom;
        
        // Hide the cursor
        ShowCursor(false);
    }

    void Main::InitializeModules() const {
        Earth::Initialize(GraphicsInstance->GetEarthBitmap());
        Satellite::Initialize(GraphicsInstance->GetSatelliteBitmap());
    }
    
    void Main::StartTicking() const {
        std::thread ticker([this] {
            // As long as the simulation is running:
            while (Running) {
                Satellite::Update();
                GraphicsInstance->Draw();
                std::this_thread::sleep_for(TickDelay);
            }

            // Important! Exit the main thread and close the window!
            SendMessage(hWnd, WM_CLOSE, NULL, NULL);
        });

        // Important! Detach the thread
        ticker.detach();
    }

    void Main::MessageLoop() const {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}