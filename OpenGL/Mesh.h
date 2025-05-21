#pragma once
#include <glad/glad.h>
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Mesh {
public:
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    void draw() const;
    void setModelMatrix(const glm::mat4& model);
    const glm::mat4& getModelMatrix() const;

private:
    GLuint VAO, VBO, EBO;
    GLsizei indexCount;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
};