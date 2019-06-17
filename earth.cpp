#include "earth.hpp"
#include "main.hpp"
#include "satellite.hpp"

int Simulation::Earth::Radius = 0;
int Simulation::Earth::RadiusOriginal = 0;
double Simulation::Earth::X = 0;
double Simulation::Earth::Y = 0;

void Simulation::Earth::Initialize(const ID2D1Bitmap* const bmp) {
    RadiusOriginal = (int)(bmp->GetSize().width / 2.0);
    Radius = RadiusOriginal;
    X = Width / 2.0;
    Y = Height / 2.0;

    Satellite::Rotation::RadiusTrajectory = 4.0 * Radius;
}