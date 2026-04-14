#include "rendering/renderer.h"
#include <glm/gtc/matrix_transform.hpp>

void Renderer::init() {
    bodyShader  = Shader("shaders/body.vert", "shaders/body.frag");
    trailShader = Shader("shaders/trail.vert", "shaders/trail.frag");
    sphere = std::make_unique<SphereMesh>(32, 32);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::drawBodies(const std::vector<CelestialBody>& bodies,
                          const Camera& cam, float aspect) {
    bodyShader.use();

    glm::mat4 view = cam.viewMatrix();
    glm::mat4 proj = cam.projectionMatrix(aspect);
    bodyShader.setMat4("view", view);
    bodyShader.setMat4("projection", proj);
    bodyShader.setVec3("viewPos", cam.getPosition());

    // Sun is always body 0 - used as the light source
    glm::vec3 sunPos(0.0f);
    if (!bodies.empty()) {
        sunPos = glm::vec3(static_cast<float>(bodies[0].position.x),
                           static_cast<float>(bodies[0].position.y),
                           static_cast<float>(bodies[0].position.z));
    }
    bodyShader.setVec3("lightPos", sunPos);

    for (size_t i = 0; i < bodies.size(); ++i) {
        const auto& b = bodies[i];
        glm::vec3 pos(static_cast<float>(b.position.x),
                      static_cast<float>(b.position.y),
                      static_cast<float>(b.position.z));

        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(model, glm::vec3(b.displayRadius));

        bodyShader.setMat4("model", model);
        bodyShader.setVec3("objectColor", b.color);
        bodyShader.setInt("isSun", (i == 0) ? 1 : 0);

        sphere->draw();
    }
}

void Renderer::drawTrails(const std::vector<Trail>& trails,
                          const Camera& cam, float aspect) {
    trailShader.use();
    glm::mat4 view = cam.viewMatrix();
    glm::mat4 proj = cam.projectionMatrix(aspect);
    trailShader.setMat4("view", view);
    trailShader.setMat4("projection", proj);

    for (const auto& trail : trails) {
        trail.draw();
    }
}
