#include "../include/module_earth.h"
#include <string>

namespace Simulation {
    long double Earth::Radius, Earth::X, Earth::Y;

    void Earth::Initialize(const ID2D1Bitmap* const bmp) {
        Radius = bmp->GetSize().width / 2.0l;
        X = Width / 2.0l;
        Y = Height / 2.0l;
    }
}