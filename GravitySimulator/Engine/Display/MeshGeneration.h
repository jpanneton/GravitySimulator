#pragma once

#include "Mesh.h"
#include <memory>

namespace MeshGeneration
{
    std::shared_ptr<Mesh> generateRevolutionObject(std::vector<glm::vec2>&& silhouettePoints, unsigned int slices, const std::vector<Texture>& textures);
    std::shared_ptr<Mesh> generateSphere(float radius, int slices, int stacks, const std::vector<Texture>& textures);
    std::shared_ptr<Mesh> generateSkybox(const std::vector<Texture>& textures);
}