#pragma once

#include <string>
#include <glm/glm.hpp>

struct CelestialBody {
    std::string name;
    double mass;               // solar masses
    glm::dvec3 position;       // AU
    glm::dvec3 velocity;       // AU/day
    glm::dvec3 acceleration;   // AU/day^2
    glm::vec3 color;           // RGB [0,1]
    float displayRadius;       // exaggerated radius in AU for rendering
};
