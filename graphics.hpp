#pragma once

#define UNICODE
#include <Windows.h>

namespace Simulation {

    class Graphics {
    public:
        Graphics(HWND hWnd);
        ~Graphics();
    };

}