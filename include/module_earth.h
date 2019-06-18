#pragma once

#include "main.h"

namespace Simulation {
    struct Earth {
        static long double Radius;
        static long double X, Y;

        static void Initialize(const ID2D1Bitmap* const bmp);
    };
}
