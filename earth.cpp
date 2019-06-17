#include "earth.hpp"
#include "main.hpp"

int Simulation::Earth::Radius = 0, Simulation::Earth::RadiusOriginal = 0;
int Simulation::Earth::X      = 0, Simulation::Earth::Y              = 0;

void Simulation::Earth::Initialize(const ID2D1Bitmap* const bmp) {
    RadiusOriginal = (int)(bmp->GetSize().width / 2.0);
    Radius = RadiusOriginal;
    X = (int)(Width / 2.0);
    Y = (int)(Height / 2.0);
}