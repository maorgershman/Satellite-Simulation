#pragma once

namespace Simulation::MagneticFields {
    struct Circular {
        static long double Radius;
        static long double DirectionX, DirectionY;
        static void Update();
    };
}