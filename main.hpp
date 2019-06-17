#pragma once

#include "graphics.hpp"
#include "event_handler.hpp"

namespace Simulation {
    extern int Width, Height;

    struct Main {
        static HWND InitializeWindow(HINSTANCE hInstance);
        static Graphics InitializeGraphics(HWND hWnd);
        static void InitializeModules(Graphics& graphics);
        static void StartUpdating();
        static void MessageLoop();
    };
}