#include "System.h"
#include <iterator>
#include <algorithm>

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
    // Run Barnes-Hut simulation (n log n)
    m_octree.buildTree(m_bodies);

    for (Body& body : m_bodies)
    {
        body.accelerate(m_octree.calculateForce(body, m_gravityFactor), timespan);
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