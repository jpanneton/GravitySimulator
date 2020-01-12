#include "MeshGeneration.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/normal.hpp>
#include <numeric>

namespace MeshGeneration
{
    std::shared_ptr<Mesh> generateRevolutionObject(std::vector<glm::vec2>&& silhouettePoints, unsigned int slices, const std::vector<Texture>& textures)
    {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        // Calculate the vertices
        for (size_t i = 0; i < silhouettePoints.size(); ++i)
        {
            float V = static_cast<float>(i) / (silhouettePoints.size() - 1);

            // Loop through meridians
            for (size_t j = 0; j <= slices; ++j)
            {
                float U = static_cast<float>(j) / slices;
                float phi = U * (glm::pi<float>() * 2);

                // Calculate the vertex position
                Vertex vertex;
                vertex.position = {
                    std::cos(phi) * silhouettePoints[i].x,
                    silhouettePoints[i].y,
                    std::sin(phi) * silhouettePoints[i].x
                };

                vertex.texCoord = { -U, V };
                vertices.push_back(std::move(vertex));
            }
        }

        std::vector<std::vector<glm::vec3>> normalsPerVertex(vertices.size());

        // Calculate the indices and normals
        for (size_t i = 0; i < silhouettePoints.size() - 1; ++i)
        {
            for (size_t j = 0; j < slices; ++j)
            {
                // Function to get a vertex index in the vertex array
                // pointIndex : index of the related 2D silhouette point (Y)
                // offset : index offset to access surrounding vertices (X)
                auto getVertexIndex = [=](size_t pointIndex, size_t offset)
                {
                    return pointIndex * (slices + 1) + offset;
                };

                // Indices calculation
                const size_t i1 = getVertexIndex(i, j);
                const size_t i2 = getVertexIndex(i + 1, (j + 1) % (slices + 1));
                const size_t i3 = getVertexIndex(i + 1, j);
                const size_t i4 = getVertexIndex(i, (j + 1) % (slices + 1));

                indices.push_back(static_cast<unsigned int>(i1));
                indices.push_back(static_cast<unsigned int>(i2));
                indices.push_back(static_cast<unsigned int>(i3));

                indices.push_back(static_cast<unsigned int>(i2));
                indices.push_back(static_cast<unsigned int>(i1));
                indices.push_back(static_cast<unsigned int>(i4));
                
                glm::vec3 normal1 = glm::triangleNormal(vertices[i1].position, vertices[i2].position, vertices[i3].position);
                glm::vec3 normal2 = glm::triangleNormal(vertices[i2].position, vertices[i1].position, vertices[i4].position);

                normalsPerVertex[i1].push_back(normal1);
                normalsPerVertex[i2].push_back(normal1);
                normalsPerVertex[i3].push_back(normal1);

                normalsPerVertex[i1].push_back(normal2);
                normalsPerVertex[i2].push_back(normal2);
                normalsPerVertex[i4].push_back(normal2);
            }
        }

        // Merge the normals of the revolution extremities
        for (size_t i = 0; i < normalsPerVertex.size(); i += slices + 1)
        {
            auto& vec1 = normalsPerVertex[i];
            auto& vec2 = normalsPerVertex[i + slices];
            vec1.insert(vec1.end(), vec2.begin(), vec2.end());
            vec2 = vec1;
        }

        // Calculate the average normal vector of each vertex
        for (size_t i = 0; i < normalsPerVertex.size(); ++i)
        {
            const std::vector<glm::vec3>& normals = normalsPerVertex[i];
            vertices[i].normal = glm::normalize(std::accumulate(normals.begin(), normals.end(), glm::vec3()));
        }

        return std::make_shared<Mesh>(std::move(vertices), std::move(indices), textures);
    }

    std::shared_ptr<Mesh> generateSphere(float radius, int slices, int stacks, const std::vector<Texture>& textures)
    {
        const float centerEpsilon = 0.0001f;
        std::vector<glm::vec2> silhouettePoints;

        // Calculate the vertices
        for (int i = 0; i <= stacks; ++i)
        {
            float stack = static_cast<float>(i) / stacks;
            // -PI/2 to PI/2
            float theta = -glm::pi<float>() / 2.f + stack * glm::pi<float>();
            silhouettePoints.push_back(glm::vec2(std::cos(theta) + centerEpsilon, std::sin(theta)) * radius);
        }

        return generateRevolutionObject(std::move(silhouettePoints), slices, textures);
    }

    std::shared_ptr<Mesh> generateSkybox(const std::vector<Texture>& textures)
    {
        std::vector<Vertex> vertices =
        {
            {{-1.0,-1.0, 1.0}},
            {{ 1.0,-1.0, 1.0}},
            {{-1.0, 1.0, 1.0}},
            {{ 1.0, 1.0, 1.0}},
            {{-1.0,-1.0,-1.0}},
            {{ 1.0,-1.0,-1.0}},
            {{-1.0, 1.0,-1.0}},
            {{ 1.0, 1.0,-1.0}}
        };

        std::vector<GLuint> indices =
        {
            // Triangle strip : 0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1,
            0, 1, 2, 1, 2, 3,
            2, 3, 7, 3, 7, 1,
            7, 1, 5, 1, 5, 4,
            5, 4, 7, 4, 7, 6,
            7, 6, 2, 6, 2, 4,
            2, 4, 0, 4, 0, 1,
        };

        return std::make_shared<Mesh>(std::move(vertices), std::move(indices), textures);
    }
}