#pragma once

#include <glad/gl.h>

class SphereMesh {
public:
    SphereMesh(int stacks = 32, int slices = 32);
    ~SphereMesh();
    void draw() const;

    SphereMesh(const SphereMesh&) = delete;
    SphereMesh& operator=(const SphereMesh&) = delete;

private:
    unsigned int vao = 0, vbo = 0, ebo = 0;
    int indexCount = 0;
};
