#pragma once

#include "rendering/shader.h"
#include "rendering/sphere_mesh.h"
#include "rendering/camera.h"
#include "rendering/trail.h"
#include "simulation/body.h"
#include <vector>
#include <memory>

class Renderer {
public:
    void init();
    void drawBodies(const std::vector<CelestialBody>& bodies, const Camera& cam, float aspect);
    void drawTrails(const std::vector<Trail>& trails, const Camera& cam, float aspect);

private:
    Shader bodyShader;
    Shader trailShader;
    std::unique_ptr<SphereMesh> sphere;
};
