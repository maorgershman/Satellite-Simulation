#include "event_handler.hpp"

LRESULT __stdcall Simulation::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return EventHandler::HandleEvents(hWnd, msg, wParam, lParam);
}

LRESULT Simulation::EventHandler::HandleEvents(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_KEYDOWN:
        HandleEventKeyDown(wParam);
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Simulation::EventHandler::HandleEventKeyDown(WPARAM key) {
    switch (key) {
    case VK_ESCAPE:
        PostQuitMessage(0);
        break;
    }
}