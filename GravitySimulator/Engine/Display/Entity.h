#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>
#include <memory>

class Entity
{
public:
    Entity() = default;
    Entity(std::shared_ptr<Mesh> mesh);
    void setMesh(std::shared_ptr<Mesh> mesh);

    glm::vec3 getPosition() const;
    void setPosition(const glm::vec3 position);
    void move(const glm::vec3 delta);
    void rotate(float angle, const glm::vec3& axis);
    void scale(float size);
    glm::mat4 getModelMatrix() const;

    void draw(GLuint shaderID, GLuint textureId, bool cubeMap = false) const;
private:
    glm::mat4 m_rotationMatrix;
    glm::mat4 m_translationMatrix;
    glm::mat4 m_scaleMatrix;
    std::shared_ptr<Mesh> m_mesh;
};