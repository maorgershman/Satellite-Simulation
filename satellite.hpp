#pragma once
#include "graphics.hpp"
#include <chrono>

namespace Simulation {
    struct Satellite {
        static int Radius, RadiusOriginal;

        static void Initialize(const ID2D1Bitmap* const bmp);

        struct Rotation {
            static std::chrono::high_resolution_clock::time_point BeginTimepoint;
        };
    };
}