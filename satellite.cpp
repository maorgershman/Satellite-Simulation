#include "satellite.hpp"
#include "earth.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

int Simulation::Satellite::Radius = 0;
int Simulation::Satellite::RadiusOriginal = 0;
double Simulation::Satellite::X = 0;
double Simulation::Satellite::Y = 0;
std::chrono::high_resolution_clock::time_point Simulation::Satellite::Rotation::BeginTimepoint;
double Simulation::Satellite::Rotation::RadiusTrajectory = 0;
double Simulation::Satellite::Rotation::AngleDegrees = 0;
double Simulation::Satellite::Rotation::AngleRadians = 0;
double Simulation::Satellite::Rotation::PeriodSeconds = 30;

void Simulation::Satellite::Initialize(const ID2D1Bitmap* const bmp) {
    RadiusOriginal = (int)(bmp->GetSize().width / 2.0);
    Radius = RadiusOriginal;

    Rotation::BeginTimepoint = std::chrono::high_resolution_clock::now();
}

void Simulation::Satellite::Update() {
    // Angle difference is proportional to the time difference!
    auto t1 = Rotation::BeginTimepoint;
    auto t2 = std::chrono::high_resolution_clock::now();
    auto diffMicros = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    auto periodMicros = Rotation::PeriodSeconds * 1000000.0;
    // Update the rotation angle
    Rotation::AngleDegrees = 360.0 * diffMicros / periodMicros;
    Rotation::AngleRadians = 2.0 * M_PI * diffMicros / periodMicros;
    if (Rotation::AngleDegrees >= 360.0 || Rotation::AngleRadians >= 2.0 * M_PI) {
        Rotation::BeginTimepoint = std::chrono::high_resolution_clock::now();
        Rotation::AngleDegrees -= 360.0;
        Rotation::AngleRadians -= 2.0 * M_PI;
    }
    // Update the location
    X = Earth::X + Rotation::RadiusTrajectory * cos(Rotation::AngleRadians);
    Y = Earth::Y - Rotation::RadiusTrajectory * sin(Rotation::AngleRadians);
}