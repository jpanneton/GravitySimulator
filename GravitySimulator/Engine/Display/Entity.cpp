#include "Entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Entity::Entity(std::shared_ptr<Mesh> mesh)
    : m_mesh(mesh)
{
}

void Entity::setMesh(std::shared_ptr<Mesh> mesh)
{
    m_mesh = mesh;
}

glm::vec3 Entity::getPosition() const
{
    return m_translationMatrix[3];
}

void Entity::setPosition(const glm::vec3 position)
{
    m_translationMatrix[3] = glm::vec4(position, 1.f);
}

void Entity::move(const glm::vec3 delta)
{
    m_translationMatrix = glm::translate(m_translationMatrix, delta);
}

void Entity::rotate(float angle, const glm::vec3& axis)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(), angle, axis);
    m_rotationMatrix = rotation * m_rotationMatrix;
}

void Entity::scale(float size)
{
    m_scaleMatrix = glm::scale(glm::mat4(), glm::vec3(size, size, size));
}

glm::mat4 Entity::getModelMatrix() const
{
    return m_translationMatrix * m_rotationMatrix * m_scaleMatrix;
}

void Entity::draw(GLuint shaderID, GLuint textureId, bool cubeMap) const
{
    assert(m_mesh != nullptr);
    
    glm::mat4 modelMatrix = getModelMatrix();
    GLint modelLocation = glGetUniformLocation(shaderID, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    m_mesh->draw(shaderID, textureId, cubeMap);
}