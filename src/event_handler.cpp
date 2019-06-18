#include "../include/event_handler.h"
#include <string>

namespace Simulation {
    LRESULT __stdcall EventHandler::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_KEYDOWN:
            HandleEventKeyDown(wParam);
            break;
        case WM_CLOSE:
            HandleEventClose();
            break;
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    void EventHandler::HandleEventKeyDown(WPARAM key) {
        switch (key) {
        case VK_ESCAPE:
            Running = false;
            break;
        }
    }

    void EventHandler::HandleEventClose() {
        PostQuitMessage(0);
    }
}