#include "Body.h"

#include <cmath>

const scalar Body::MassMin = 1.0f;
const scalar Body::VelocityMin = 0.0f;

Body::Body(const vec3& position, const vec3& velocity, scalar mass, Material material)
    : m_position{ position }
    , m_velocity{ velocity }
    , m_mass{ mass }
    , m_radius{ radiusFromMass(mass) }
    , m_material { material }
{
}

Body::Body(const Body & other)
    : m_position{ other.m_position }
    , m_velocity{ other.m_velocity }
    , m_mass{ other.m_mass }
    , m_radius{ other.m_radius }
    , m_material { other.m_material }
{
}

const vec3& Body::position() const noexcept
{
    return m_position;
}

const vec3& Body::velocity() const noexcept
{
    return m_velocity;
}

scalar Body::mass() const noexcept
{
    return m_mass;
}

scalar Body::radius() const noexcept
{
    return m_radius;
}

Material Body::material() const noexcept
{
    return m_material;
}

void Body::move(scalar dt)
{
    m_position += dt * m_velocity;
}

void Body::accelerate(const vec3 & dv, scalar dt)
{
    m_velocity += dt * dv;
}

scalar Body::radiusFromMass(scalar mass)
{
    return std::cbrt((3.0f * mass) / (4.0f * PI));
}
