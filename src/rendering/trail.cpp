#include "rendering/trail.h"
#include <cstring>
#include <utility>

Trail::Trail(int maxPoints) : maxPts(maxPoints) {
    buffer.resize(maxPts * 7, 0.0f);
    init();
}

Trail::~Trail() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
}

Trail::Trail(Trail&& o) noexcept
    : buffer(std::move(o.buffer)), color(o.color), maxPts(o.maxPts),
      head(o.head), count(o.count), vao(o.vao), vbo(o.vbo) {
    o.vao = 0; o.vbo = 0;
}

Trail& Trail::operator=(Trail&& o) noexcept {
    if (this != &o) {
        if (vao) glDeleteVertexArrays(1, &vao);
        if (vbo) glDeleteBuffers(1, &vbo);
        buffer = std::move(o.buffer);
        color = o.color; maxPts = o.maxPts;
        head = o.head; count = o.count;
        vao = o.vao; vbo = o.vbo;
        o.vao = 0; o.vbo = 0;
    }
    return *this;
}

void Trail::init() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color+alpha
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Trail::addPoint(const glm::vec3& p) {
    int idx = head * 7;
    buffer[idx + 0] = p.x;
    buffer[idx + 1] = p.y;
    buffer[idx + 2] = p.z;
    buffer[idx + 3] = color.r;
    buffer[idx + 4] = color.g;
    buffer[idx + 5] = color.b;
    buffer[idx + 6] = 1.0f; // alpha set during draw reordering

    head = (head + 1) % maxPts;
    if (count < maxPts) ++count;
}

void Trail::draw() const {
    if (count < 2) return;

    // Build ordered vertex data with alpha fade
    std::vector<float> ordered(count * 7);
    int start = (head - count + maxPts) % maxPts;
    for (int i = 0; i < count; ++i) {
        int srcIdx = ((start + i) % maxPts) * 7;
        int dstIdx = i * 7;
        float alpha = static_cast<float>(i) / static_cast<float>(count - 1);
        ordered[dstIdx + 0] = buffer[srcIdx + 0];
        ordered[dstIdx + 1] = buffer[srcIdx + 1];
        ordered[dstIdx + 2] = buffer[srcIdx + 2];
        ordered[dstIdx + 3] = buffer[srcIdx + 3];
        ordered[dstIdx + 4] = buffer[srcIdx + 4];
        ordered[dstIdx + 5] = buffer[srcIdx + 5];
        ordered[dstIdx + 6] = alpha;
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, ordered.size() * sizeof(float), ordered.data());
    glDrawArrays(GL_LINE_STRIP, 0, count);
    glBindVertexArray(0);
}
