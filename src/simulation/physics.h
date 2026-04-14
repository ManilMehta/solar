#pragma once

#include "body.h"
#include <vector>

constexpr double G_CONST = 2.9591220828e-4; // AU^3 / (M_sun * day^2)
constexpr double SOFTENING = 1.0e-12;       // AU^2, prevents division by zero

class Physics {
public:
    void computeAccelerations(std::vector<CelestialBody>& bodies);
    void step(std::vector<CelestialBody>& bodies, double dt);
};
