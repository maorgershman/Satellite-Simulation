#pragma once

#define UNICODE
#include <Windows.h>

namespace Simulation {
    LRESULT __stdcall WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    class EventHandler {
    private:
        static void HandleEventKeyDown(WPARAM key);
    public:
        static LRESULT HandleEvents(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };
}