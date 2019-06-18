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

    extern int Width, Height;
    extern bool Running;

    class Graphics;

    class Main {
    private:
        static constexpr auto TicksPerSecond = 120;
        static constexpr auto TickDelay = std::chrono::microseconds((int)(1000000.0 / TicksPerSecond));

        HINSTANCE hInstance;
        HWND hWnd;

        void InitializeWindow();
        void InitializeModules(const ID2D1Bitmap* const bmpEarth, const ID2D1Bitmap* const bmpSatellite) const;
        void StartTicking(Graphics& graphics) const;
        static void Tick(Graphics& graphics, HWND hWnd);
        void MessageLoop() const;
    public:
        int ExitCode;

        Main(HINSTANCE hInstance);
    };
}