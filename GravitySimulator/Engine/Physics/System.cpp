#include "System.h"
#include "Engine/Core/ThreadPool.h"
#include <iterator>
#include <algorithm>

namespace
{
    ThreadPool pool(2); // Two workers : one for force calculation and one for collision detection
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
{
    m_collisions.reserve(BodiesArray::MAX_BODIES * 2);
}

System::System(const System& other)
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
    const scalar timespan = m_timescale * dt.asSeconds();

    // Run Barnes-Hut simulation (n log n)
    m_octree.buildTree(m_bodies);

    pool.enqueue([&] { applyGravity(timespan); });
    pool.enqueue([&] { detectCollisions(); });
    pool.waitFinished();

    moveAllBodies(timespan);
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
    // Reversed loop to avoid false sharing with collision detection thread
    for (size_t i = m_bodies.size(); i-- > 0;)
    {
        Body& body = m_bodies[static_cast<int32_t>(i)];
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

    for (int32_t i = 0; i < m_bodies.size(); ++i)
    {
        const int32_t result = m_octree.detectCollision(m_bodies[i], i);
        if (result != -1)
        {
            assert(result != i);
            m_collisions.push_back({ i, result });
        }
    }
}

void System::resolveCollisions()
{
    for (auto collision : m_collisions)
    {
        m_bodies.merge(begin() + collision.first, begin() + collision.second);
    }
}