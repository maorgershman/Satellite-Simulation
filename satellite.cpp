#include "satellite.hpp"

int Simulation::Satellite::Radius = 0, Simulation::Satellite::RadiusOriginal = 0;
std::chrono::high_resolution_clock::time_point Simulation::Satellite::Rotation::BeginTimepoint;

void Simulation::Satellite::Initialize(const ID2D1Bitmap* const bmp) {
    RadiusOriginal = (int)(bmp->GetSize().width / 2.0);
    Radius = RadiusOriginal;

    Rotation::BeginTimepoint = std::chrono::high_resolution_clock::now();
}