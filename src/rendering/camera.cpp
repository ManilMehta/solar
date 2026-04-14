#include "rendering/camera.h"
#include <algorithm>
#include <cmath>

Camera::Camera() {}

glm::mat4 Camera::viewMatrix() const {
    glm::vec3 pos = getPosition();
    return glm::lookAt(pos, focusPoint, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::projectionMatrix(float aspect) const {
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

void Camera::rotate(float dx, float dy) {
    yaw   += dx * 0.005f;
    pitch += dy * 0.005f;
    pitch = std::clamp(pitch, -1.5f, 1.5f);
}

void Camera::zoom(float delta) {
    distance *= std::pow(1.1f, -delta);
    distance = std::clamp(distance, 0.01f, 500.0f);
}

void Camera::setFocus(const glm::dvec3& target) {
    focusPoint = glm::vec3(static_cast<float>(target.x),
                           static_cast<float>(target.y),
                           static_cast<float>(target.z));
}

glm::vec3 Camera::getPosition() const {
    float x = distance * std::cos(pitch) * std::sin(yaw);
    float y = distance * std::sin(pitch);
    float z = distance * std::cos(pitch) * std::cos(yaw);
    return focusPoint + glm::vec3(x, y, z);
}
