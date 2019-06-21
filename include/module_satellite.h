#pragma once

#include "main.h"

namespace Simulation {
    class Satellite {
    private:
        static constexpr auto DefaultPeriodSeconds = 30.0l;
    public:
        static long double Radius;
        static long double RadiusTrajectory;
        static long double X, Y;
        static long double PeriodSeconds;
        static long double AngleDegrees, AngleRadians;
        static Timepoint RotationBeginTimepoint;

        static void Initialize(const ID2D1Bitmap* const);

        static void Update();
    };
}
