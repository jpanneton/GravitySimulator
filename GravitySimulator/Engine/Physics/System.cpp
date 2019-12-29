#include "System.h"
#include <iterator>
#include <algorithm>
#include <glm/gtx/component_wise.hpp>

namespace
{
    template<typename T, class Func>
    inline T roundToPowerOfTwo(T value, Func roundingFunction)
    {
        const T sign = value < T(0) ? T(-1) : T(1);
        value = std::abs(value);
        return sign * (value > T(2) ?
            std::exp2(roundingFunction(std::log2(value))) :
            roundingFunction(value));
    }
}

System::System(scalar gravityFactor)
    : System{ {}, gravityFactor }
{
}

System::System(const BodiesArray& bodies, scalar gravityFactor, scalar timescale)
    : m_bodies{ bodies }
    , m_gravityFactor{ gravityFactor }
    , m_timescale{ timescale }
    , m_timestep{ 0.1f } 
    , m_collisions{}
{
    m_collisions.reserve(BodiesArray::MAX_BODIES * 2);
}

System::System(const System & other)
    : System{ other.m_bodies, other.m_gravityFactor, other.m_timescale }
{
}

System::iterator System::begin()
{
    return m_bodies.begin();
}

System::iterator System::end()
{
    return m_bodies.end();
}

void System::update(sf::Time dt)
{
    scalar timespan = m_timescale * dt.asSeconds();
    applyGravity(timespan);
    moveAllBodies(timespan);
    detectCollisions();
    resolveCollisions();
}

void System::addBody(const Body& body)
{
    m_bodies.push_back(body);
}

scalar System::timescale() const
{
    return m_timescale;
}

void System::increaseTimescale()
{
    m_timescale += m_timestep;
}

void System::decreaseTimescale()
{
    m_timescale -= m_timestep;
    if (m_timescale <= 0.f)
        m_timescale = m_timestep;
}

void System::save(std::ostream& os)
{
    serializeBodies(os, m_bodies);
}

void System::applyGravity(float timespan)
{
    // Calculate world bounds
    glm::vec3 minWorldPoint(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    glm::vec3 maxWorldPoint(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

    for (Body& body : m_bodies)
    {
        const glm::vec3& bodyPosition = body.position();
        if (bodyPosition.x < minWorldPoint.x) minWorldPoint.x = bodyPosition.x;
        if (bodyPosition.x > maxWorldPoint.x) maxWorldPoint.x = bodyPosition.x;
        if (bodyPosition.y < minWorldPoint.y) minWorldPoint.y = bodyPosition.y;
        if (bodyPosition.y > maxWorldPoint.y) maxWorldPoint.y = bodyPosition.y;
        if (bodyPosition.z < minWorldPoint.z) minWorldPoint.z = bodyPosition.z;
        if (bodyPosition.z > maxWorldPoint.z) maxWorldPoint.z = bodyPosition.z;
    }

    const glm::vec3 worldRadius = 0.5f * (maxWorldPoint - minWorldPoint);
    const glm::vec3 worldCenter = minWorldPoint + worldRadius;

    // Round world center coordinates to closest powers of 2
    const glm::vec3 newWorldCenter = { roundToPowerOfTwo(worldCenter.x, std::roundf), roundToPowerOfTwo(worldCenter.y, std::roundf), roundToPowerOfTwo(worldCenter.z, std::roundf) };
    // Ceil world radius to closest power of 2 while taking into account the new center offset
    const float newWorldRadius = roundToPowerOfTwo(glm::compMax(worldRadius + glm::abs(newWorldCenter - worldCenter)), std::ceilf);

    // Run Barnes-Hut simulation (n log n)
    // Since bounds are powers of 2, floating-point errors are avoided when dividing the space
    BarnesHutOctree octree({ newWorldCenter, newWorldRadius });
    octree.buildTree(m_bodies);

    for (Body& body : m_bodies)
    {
        body.accelerate(octree.calculateForce(body, m_gravityFactor), timespan);
    }
}

void System::moveAllBodies(float timespan)
{
    for (Body& body : m_bodies)
    {
        body.move(timespan);
    }
}

void System::detectCollisions()
{
    m_collisions.clear();

    for (auto first = begin(); first != end(); ++first)
    {
        for (auto second = std::next(first); second != end(); ++second)
        {
            if (first->collidesWith(*second))
            {
                m_collisions.push_back({ first, second });
            }
        }
    }
}

void System::resolveCollisions()
{
    for (auto collision : m_collisions)
    {
        m_bodies.merge(collision.first, collision.second);
    }
}