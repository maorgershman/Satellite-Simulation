#include "../include/main.h"

int __stdcall wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
    return Simulation::MainInstance.Run(hInstance);
}
