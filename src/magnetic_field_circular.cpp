#include "../include/magnetic_field_circular.h"
#include "../include/module_satellite.h"
#include "../include/module_earth.h"

namespace Simulation::MagneticFields {
    long double Circular::Radius, Circular::DirectionX, Circular::DirectionY;

    void Circular::Update() {
        auto deg = Satellite::AngleDegrees;
        auto rad = Satellite::AngleRadians;
        auto r = Satellite::RadiusTrajectory / (2.0l * cos(rad));
        if (deg == 0.0l || deg == 180.0l) {
            DirectionX = 0.0l;
            DirectionY = 1.0l;
        }
        else if (deg == 90.0l || deg == 270.0l) {
            DirectionX = 0.0l;
            DirectionY = -2.0l;
        }
        else {
            auto x = Satellite::X - Earth::X;
            auto m = (r - x) / sqrt(-x * (x - 2.0l * r));
            auto kx = ((deg > 0.0l  && deg < 90.0l)  || (deg > 180.0l && deg < 270.0l)) ? -1 : 1;
            auto ky = ((deg > 45.0l && deg < 135.0l) || (deg > 225.0l && deg < 315.0l)) ? -1 : 1;
            auto _x = 1.0l / sqrt(1.0l + pow(m, 2.0l));
            auto _y = abs(m) * _x;
            auto coef = sqrt(1.0l + 3.0l * pow(sin(rad), 2.0l));
            DirectionX = _x * kx * coef;
            DirectionY = _y * ky * coef;
        }
        Radius = abs(r * 2.0l);
    }
}