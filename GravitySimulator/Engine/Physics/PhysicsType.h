#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>

using scalar = float;
using vec3 = glm::vec3;

constexpr scalar PI = glm::pi<scalar>();

enum Material
{
    ceres = 0,
    earth,
    eris,
    haumea,
    jupiter,
    makemake,
    mars,
    mercury,
    moon,
    neptune,
    saturn,
    sun,
    uranus,
    venus,
    NB_MATERIALS
};

const char* MaterialNames[];