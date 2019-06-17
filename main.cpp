#include "main.hpp"

int __stdcall wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
    HWND hWnd = Simulation::Main::InitializeWindow(hInstance);
    Simulation::Graphics graphics = Simulation::Main::InitializeGraphics(hWnd);
    Simulation::Main::StartUpdating();
    Simulation::Main::MessageLoop();
    return 0;
}

HWND Simulation::Main::InitializeWindow(HINSTANCE hInstance) {
    return nullptr;
}

Simulation::Graphics Simulation::Main::InitializeGraphics(HWND hWnd) {
    return Graphics(hWnd);
}

void Simulation::Main::StartUpdating() {

}

void Simulation::Main::MessageLoop() {

}
