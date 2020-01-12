#include "Mesh.h"
#include <cassert>
#include <numeric>
#include <string>

Mesh::Mesh(std::vector<Vertex>&& vertices, const std::vector<Texture>& textures)
    : m_vertices(std::move(vertices))
    , m_indices(m_vertices.size())
    , m_textures(textures)
{
    std::iota(std::begin(m_indices), std::end(m_indices), 0);
    initBuffers();
}

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices, const std::vector<Texture>& textures)
    : m_vertices(std::move(vertices))
    , m_indices(std::move(indices))
    , m_textures(textures)
{
    initBuffers();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vaoID);
    glDeleteBuffers(1, &m_vboID);
    glDeleteBuffers(1, &m_eboID);
}

void Mesh::initBuffers()
{
    // Buffers generation
    glGenVertexArrays(1, &m_vaoID);
    glGenBuffers(1, &m_vboID);
    glGenBuffers(1, &m_eboID);

    glBindVertexArray(m_vaoID);

    // Sending data to buffers
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);

    // Position attribute binding
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

    // Normal attribute binding
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

    // TexCoord attribute binding
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord));

    glBindVertexArray(0);
}

void Mesh::initCopy(GLuint shaderID, GLuint textureId, bool cubeMap) const
{
    // Textures binding
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(cubeMap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, m_textures[textureId].id());

    std::string uniformName = "texture" + std::to_string(textureId);
    glUniform1i(glGetUniformLocation(shaderID, uniformName.c_str()), textureId);

    glBindVertexArray(m_vaoID);
}

void Mesh::drawCopy(GLuint shaderID) const
{
    // Draw mesh
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);
}

void Mesh::draw(GLuint shaderID, GLuint textureId, bool cubeMap) const
{
    initCopy(shaderID, textureId,cubeMap);
    drawCopy(shaderID);
}