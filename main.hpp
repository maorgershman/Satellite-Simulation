#pragma once

#include "graphics.h"

namespace Simulation {
    struct Main {
        static HWND InitializeWindow(HINSTANCE hInstance);
        static Graphics InitializeGraphics(HWND hWnd);
        static void StartUpdating();
        static void MessageLoop();
    };
}