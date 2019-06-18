#pragma once

#include "main.h"

namespace Simulation {
    class EventHandler {
    private:
        static void HandleEventKeyDown(WPARAM key);
        static void HandleEventClose();
    public:
        static LRESULT __stdcall WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };
}