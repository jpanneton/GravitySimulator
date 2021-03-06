#include "Body.h"
#include <algorithm>

Body::Body(const vec3& position, const vec3& velocity, scalar mass, Material material)
    : m_position{ position }
    , m_velocity{ velocity }
    , m_mass{ std::max(MassMin, mass) }
    , m_radius{ radiusFromMass(mass) }
    , m_material { material }
{
}

Body::Body(const Body& other)
    : Body(other.m_position, other.m_velocity, other.m_mass, other.m_material)
{
}

const vec3& Body::getPosition() const noexcept
{
    return m_position;
}

const vec3& Body::getVelocity() const noexcept
{
    return m_velocity;
}

scalar Body::getMass() const noexcept
{
    return m_mass;
}

scalar Body::getRadius() const noexcept
{
    return m_radius;
}

Material Body::getMaterial() const noexcept
{
    return m_material;
}

void Body::move(scalar dt)
{
    m_position += dt * m_velocity;
}

void Body::accelerate(const vec3& dv, scalar dt)
{
    m_velocity += dt * dv;
}

bool Body::collidesWith(const Body& other) const
{
    return glm::distance(getPosition(), other.getPosition()) <= getRadius() + other.getRadius();
}

void Body::kill()
{
    m_mass = 0.0f;
}

bool Body::isDead() const
{
    return m_mass == 0.0f;
}

scalar Body::radiusFromMass(scalar mass)
{
    return std::cbrt((3.0f * mass) / (4.0f * PI));
}