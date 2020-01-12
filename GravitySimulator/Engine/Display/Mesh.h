#pragma once

#include "Texture.h"
#include <glm/glm.hpp>
#include <vector>

class Texture;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex>&& vertices, const std::vector<Texture>& textures);
    Mesh(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices, const std::vector<Texture>& textures);
    ~Mesh();

    void initCopy(GLuint shaderID, GLuint textureId, bool cubeMap) const;
    void drawCopy(GLuint shaderID) const;
    void draw(GLuint shaderID, GLuint textureId, bool cubeMap) const;
private:
    GLuint m_vaoID;
    GLuint m_vboID;
    GLuint m_eboID;
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;
    const std::vector<Texture>& m_textures;
    void initBuffers();
};