#include "System.h"
#include <iterator>
#include <algorithm>

System::System(const PhysicsEngine& engine)
    : System{ {}, engine }
{
}

System::System(const BodiesArray& bodies, const PhysicsEngine& engine, scalar timescale)
    : m_bodies{ bodies }
    , m_physicsEngine{ engine }
    , m_timescale{ timescale }
    , m_timestep{ 0.1f } 
    , m_collisions{}
{
    m_collisions.reserve(BodiesArray::MAX_BODIES * 2);
}

System::System(const System & other)
    : System{ other.m_bodies, other.m_physicsEngine, other.m_timescale }
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

void System::addBody(const Body & body)
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
    auto& engine = m_physicsEngine;
    for_each_distinct_pair(begin(), end(), [&engine, timespan](Body& first, Body& second) {
        engine.applyGravity(first, second, timespan);
    });
}

void System::moveAllBodies(float timespan)
{
    std::for_each(begin(), end(), [timespan](Body& body) {
        body.move(timespan);
    });
}

void System::detectCollisions()
{
    m_collisions.clear();

    for (auto first = begin(); first != end(); ++first)
    {
        for (auto second = std::next(first); second != end(); ++second)
        {
            if (m_physicsEngine.detectCollision(*first, *second))
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