#pragma once
#include "graphics.hpp"

namespace Simulation {
    struct Earth {
        static int Radius, RadiusOriginal;
        static double X, Y;

        static void Initialize(const ID2D1Bitmap* const bmp);
    };
}