#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    unsigned int id = 0;

    Shader() = default;
    Shader(const std::string& vertPath, const std::string& fragPath);
    void use() const;
    void setMat4(const std::string& name, const glm::mat4& m) const;
    void setVec3(const std::string& name, const glm::vec3& v) const;
    void setFloat(const std::string& name, float v) const;
    void setInt(const std::string& name, int v) const;

private:
    unsigned int compile(const std::string& path, GLenum type);
};
