#include "PhysicsEngine.h"
#include <glm/gtx/norm.hpp>

PhysicsEngine::PhysicsEngine()
    : PhysicsEngine{ 1.0f }
{
}

PhysicsEngine::PhysicsEngine(scalar g)
    : G{ g }
{
}

PhysicsEngine::PhysicsEngine(const PhysicsEngine & other)
    : PhysicsEngine{ other.G }
{
}

void PhysicsEngine::applyGravity(Body& first, Body& second, float timespan) const
{
    vec3 gravityVector = second.position() - first.position();
    vec3 normalizedGravityVector = glm::normalize(gravityVector);
    scalar distance_squared = glm::length2(gravityVector);

    first.accelerate(gravityMagnitude(second.mass(), distance_squared) * normalizedGravityVector, timespan);
    second.accelerate(-gravityMagnitude(first.mass(), distance_squared) * normalizedGravityVector, timespan);
}

bool PhysicsEngine::detectCollision(const Body & first, const Body & second) const
{
    auto distance = glm::length(first.position() - second.position());
    return distance <= first.radius() + second.radius();
}

scalar PhysicsEngine::gravityMagnitude(scalar mass, scalar distance_squared) const
{
    return G * (mass / distance_squared);
}
