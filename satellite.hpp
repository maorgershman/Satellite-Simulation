#pragma once
#include "graphics.hpp"
#include <chrono>

namespace Simulation {
    struct Satellite {
        static int Radius, RadiusOriginal;
        static double X, Y;

        static void Initialize(const ID2D1Bitmap* const bmp);
        static void Update();

        struct Rotation {
            static std::chrono::high_resolution_clock::time_point BeginTimepoint;
            static double RadiusTrajectory;
            static double AngleDegrees, AngleRadians;
            static double PeriodSeconds;
        };
    };
}