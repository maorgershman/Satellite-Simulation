#include "../include/event_handler.h"
#include "../include/module_satellite.h"
#include "../include/main.h"
#include "../include/graphics.h"

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

    inline bool IsKeyDown(int key) {
        return GetAsyncKeyState(key) & (1 << 15);
    }

    void EventHandler::HandleEventKeyDown(WPARAM key) {
        switch (key) {
        case VK_OEM_MINUS:
            HandleEventKeyMinusPressed();
            break;
        case VK_OEM_PLUS:
            HandleEventKeyPlusPressed();
            break;
        case VK_ESCAPE:
            Running = false;
            break;
        }
    }

    void EventHandler::HandleEventKeyMinusPressed() {
        if (IsKeyDown('T')) {
            // Decrease the period
            if (Satellite::PeriodSeconds > 5.0l) {
                Satellite::PeriodSeconds -= 5.0l;
                Satellite::RotationBeginTimepoint = Now();
                GraphicsInstance->UpdateTextLayouts();
            }
        }
    }

    void EventHandler::HandleEventKeyPlusPressed() {
        if (IsKeyDown('T')) {
            // Increase the period
            Satellite::PeriodSeconds += 5.0l;
            Satellite::RotationBeginTimepoint = Now();
            GraphicsInstance->UpdateTextLayouts();
        }
    }

    void EventHandler::HandleEventClose() {
        PostQuitMessage(0);
    }
}