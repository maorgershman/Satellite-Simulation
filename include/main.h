#pragma once

#define UNICODE
#include <Windows.h>
#include <d2d1.h>

#include <cmath>
#include <chrono>

namespace Simulation {
    constexpr auto PI = 3.141592653589793l;

    using Timepoint = std::chrono::high_resolution_clock::time_point;

    static inline auto Now() {
        return std::chrono::high_resolution_clock::now();
    }

    class Main {
        static constexpr auto TicksPerSecond = 200;
        static constexpr auto TickDelay = std::chrono::microseconds((int)(1000000.0 / TicksPerSecond));
    private:
        HWND hWnd;

        inline void InitializeWindow(HINSTANCE);
        inline void InitializeModules() const;
        inline void StartTicking() const;
        inline void MessageLoop() const;
    public:
        int Run(HINSTANCE);
    };

    class Graphics;

    extern int Width, Height;
    extern bool Running;
    extern Main MainInstance;
    extern Graphics* GraphicsInstance;
}