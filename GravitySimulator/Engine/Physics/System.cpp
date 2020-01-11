#include "System.h"
#include "Engine/Core/ThreadPool.h"
#include <iterator>
#include <algorithm>

namespace
{
    const unsigned int WORKER_COUNT = std::thread::hardware_concurrency();
    // Determines how the data should be splitted for asynchronous computations
    // Two workers per batch : one for force calculation and one for collision detection
    const unsigned int BATCH_COUNT = WORKER_COUNT > 1 ? WORKER_COUNT / 2 : WORKER_COUNT;
    ThreadPool pool(WORKER_COUNT);
}

System::System(scalar gravityFactor)
    : System{ {}, gravityFactor }
{
    assert(2 * BATCH_COUNT == WORKER_COUNT || BATCH_COUNT == WORKER_COUNT);
}

System::System(const BodiesArray& bodies, scalar gravityFactor, scalar timescale)
    : m_bodies{ bodies }
    , m_collisionBatches(BATCH_COUNT)
    , m_gravityFactor{ gravityFactor }
    , m_timescale{ timescale }
    , m_timestep{ 0.1f }
{
    for (auto& collisionBatch : m_collisionBatches)
    {
        collisionBatch.reserve(BodiesArray::MAX_BODIES / BATCH_COUNT);
    }
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

    for (unsigned int batchIndex = 0; batchIndex < BATCH_COUNT; ++batchIndex)
    {
        pool.enqueue([=] { applyGravity(batchIndex, timespan); });
        pool.enqueue([=] { detectCollisions(batchIndex); });
    }

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

void System::applyGravity(unsigned int batchIndex, float timespan)
{
    const auto indexRange = ThreadPool::getRangeFromBatch(m_bodies.size(), BATCH_COUNT, batchIndex);

    // Reversed loop to avoid false sharing with collision detection thread
    for (auto i = indexRange.second; i-- > indexRange.first;)
    {
        Body& body = m_bodies[i];
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

void System::detectCollisions(unsigned int batchIndex)
{
    const auto indexRange = ThreadPool::getRangeFromBatch(m_bodies.size(), BATCH_COUNT, batchIndex);
    auto& collisionBatch = m_collisionBatches[batchIndex];

    for (auto i = indexRange.first; i < indexRange.second; ++i)
    {
        const int32_t result = m_octree.detectCollision(m_bodies[i], i);
        if (result != -1)
        {
            assert(result != i);
            collisionBatch.push_back({ i, result });
        }
    }
}

void System::resolveCollisions()
{
    for (auto& collisionBatch : m_collisionBatches)
    {
        for (const auto& collision : collisionBatch)
        {
            m_bodies.merge(begin() + collision.first, begin() + collision.second);
        }
        collisionBatch.clear();
    }

    m_bodies.removeDeadBodies();
}