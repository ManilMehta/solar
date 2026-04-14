#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();

    glm::mat4 viewMatrix() const;
    glm::mat4 projectionMatrix(float aspect) const;

    void rotate(float dx, float dy);
    void zoom(float delta);
    void setFocus(const glm::dvec3& target);

    glm::vec3 getPosition() const;

    float yaw   = 0.0f;
    float pitch  = 0.3f;
    float distance = 5.0f;
    glm::vec3 focusPoint{0.0f};

    float nearPlane = 0.001f;
    float farPlane  = 2000.0f;
    float fov       = 45.0f;
};
