#pragma once

#include "graphics.hpp"
#include "event_handler.hpp"

namespace Simulation {
    constexpr int UpdatesPerSecond = 200;
    static_assert(UpdatesPerSecond <= 1000000);

    extern int Width, Height;
    extern bool Running;

    class Main {
    private:
        static void Tick(Graphics& graphics);
    public:
        static HWND InitializeWindow(HINSTANCE hInstance);
        static Graphics InitializeGraphics(HWND hWnd);
        static void InitializeModules(Graphics& graphics);
        static void StartUpdating(Graphics& graphics);
        static void MessageLoop();
    };
}