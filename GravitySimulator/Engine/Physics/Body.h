#pragma once

#include "PhysicsType.h"

class Body
{
    vec3 m_position;
    vec3 m_velocity;
    scalar m_mass;
    scalar m_radius;
    Material m_material;

public:
    static constexpr scalar MassMin = 1.0f;
    static constexpr scalar VelocityMin = 0.0f;

    Body() = default;
    Body(const vec3& position, const vec3& velocity, scalar mass, Material material);
    Body(const Body& other);

    const vec3& position() const noexcept;
    const vec3& velocity() const noexcept;
    scalar mass() const noexcept;
    scalar radius() const noexcept;
    Material material() const noexcept;

    void move(scalar dt);
    void accelerate(const vec3& dv, scalar dt);
    bool collidesWith(const Body& other) const;

    static scalar radiusFromMass(scalar mass);
};