#include "simulation/physics.h"
#include <cmath>

void Physics::computeAccelerations(std::vector<CelestialBody>& bodies) {
    for (auto& b : bodies)
        b.acceleration = glm::dvec3(0.0);

    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            glm::dvec3 r = bodies[j].position - bodies[i].position;
            double dist2 = glm::dot(r, r) + SOFTENING;
            double dist  = std::sqrt(dist2);
            double inv3  = 1.0 / (dist2 * dist);

            glm::dvec3 force_dir = r * inv3;
            bodies[i].acceleration += G_CONST * bodies[j].mass * force_dir;
            bodies[j].acceleration -= G_CONST * bodies[i].mass * force_dir;
        }
    }
}

void Physics::step(std::vector<CelestialBody>& bodies, double dt) {
    // Velocity Verlet integration
    for (auto& b : bodies) {
        b.position += b.velocity * dt + 0.5 * b.acceleration * dt * dt;
    }

    // Store old accelerations implicitly by half-updating velocity first
    std::vector<glm::dvec3> oldAcc(bodies.size());
    for (size_t i = 0; i < bodies.size(); ++i)
        oldAcc[i] = bodies[i].acceleration;

    computeAccelerations(bodies);

    for (size_t i = 0; i < bodies.size(); ++i) {
        bodies[i].velocity += 0.5 * (oldAcc[i] + bodies[i].acceleration) * dt;
    }
}
