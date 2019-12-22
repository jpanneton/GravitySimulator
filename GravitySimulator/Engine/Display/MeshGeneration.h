#pragma once

#include "Mesh.h"
#include <glm\gtc\constants.hpp>
#include <memory>
#include <vector>

namespace MeshGeneration
{
    std::shared_ptr<Mesh> generateCube(std::vector<Texture> textures);
    std::shared_ptr<Mesh> generateSkyBox(std::vector<Texture> textures);
    std::shared_ptr<Mesh> generateSphere(std::vector<Texture> textures);
}