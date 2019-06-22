#pragma once

#include "main.h"

namespace Simulation {
    class Satellite {
        static constexpr auto DefaultPeriodSeconds = 30.0l;
    public:
        static long double Radius;
        static long double RadiusTrajectory;
        static long double X, Y;
        static long double PeriodSeconds;
        static long double AngleDegrees, AngleRadians;
        static long double RadialDirectionX, RadialDirectionY;
        static long double TangentDirectionX, TangentDirectionY;
        static Timepoint RotationBeginTimepoint;

        static void Initialize(const ID2D1Bitmap* const);

        static void Update();
    };
}
