#include "../include/module_satellite.h"
#include "../include/module_earth.h"

namespace Simulation {
    long double Satellite::Radius, Satellite::RadiusTrajectory, Satellite::X, Satellite::Y, Satellite::AngleDegrees, Satellite::AngleRadians;
    long double Satellite::PeriodSeconds = DefaultPeriodSeconds;
    Timepoint Satellite::RotationBeginTimepoint;

    void Satellite::Initialize(const ID2D1Bitmap* const bmp) {
        Radius = bmp->GetSize().width / 2.0l;
        RadiusTrajectory = 4.0l * Earth::Radius;
        RotationBeginTimepoint = Now();
    }

    void Satellite::Update() {
        auto diffMicros = std::chrono::duration_cast<std::chrono::microseconds>(Now() - RotationBeginTimepoint).count();
        auto periodMicros = PeriodSeconds * 1000000.0l;

        // Update the rotation angle
        AngleDegrees = 360.0l * diffMicros / periodMicros;
        AngleRadians = 2.0l * PI * diffMicros / periodMicros;
        if (AngleDegrees >= 360.0l || AngleRadians >= 2.0l * PI) {
            RotationBeginTimepoint = Now();
            AngleDegrees -= 360.0l;
            AngleRadians -= 2.0l * PI;
        }

        // Update the location
        X = Earth::X + RadiusTrajectory * cos(AngleRadians);
        Y = Earth::Y - RadiusTrajectory * sin(AngleRadians);
    }
}