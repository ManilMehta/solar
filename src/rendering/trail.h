#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>

class Trail {
public:
    Trail(int maxPoints = 2000);
    ~Trail();

    void addPoint(const glm::vec3& p);
    void draw() const;
    void setColor(const glm::vec3& c) { color = c; }

    Trail(const Trail&) = delete;
    Trail& operator=(const Trail&) = delete;
    Trail(Trail&& o) noexcept;
    Trail& operator=(Trail&& o) noexcept;

private:
    void init();

    std::vector<float> buffer; // interleaved: x,y,z,r,g,b,a
    glm::vec3 color{1.0f};
    int maxPts;
    int head = 0;
    int count = 0;
    unsigned int vao = 0, vbo = 0;
};
