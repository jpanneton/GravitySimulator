#include "BodiesArray.h"

BodiesArray::BodiesArray()
{
    m_bodies.reserve(MAX_BODIES);
}

void BodiesArray::push_back(const Body& body)
{
    if (m_bodies.size() < MAX_BODIES)
    {
        m_bodies.push_back(body);
    }
}

void BodiesArray::merge(iterator target, iterator source)
{
    auto totalMass = target->mass() + source->mass();
    auto t = target->mass() / totalMass;
    auto s = source->mass() / totalMass;

    vec3 newPosition = t * target->position() + s * source->position();
    vec3 newVelocity = t * target->velocity() + s * source->velocity();
    Material newMaterial = t > s ? target->material() : source->material();

    *target = Body{ newPosition, newVelocity, totalMass, newMaterial };
    source->kill();
}

void BodiesArray::removeDeadBodies()
{
    while (m_bodies.back().isDead())
        m_bodies.pop_back();

    for (int32_t i = 0; i < m_bodies.size(); ++i)
    {
        if (m_bodies[i].isDead())
        {
            assert(!m_bodies.back().isDead());
            m_bodies[i] = m_bodies.back();
            m_bodies.pop_back();
        }

        while (m_bodies.back().isDead())
            m_bodies.pop_back();
    }
}

size_t BodiesArray::size() const
{
    return m_bodies.size();
}

Body& BodiesArray::operator[](int32_t n)
{
    return m_bodies[n];
}

const Body& BodiesArray::operator[](int32_t n) const
{
    return m_bodies[n];
}

BodiesArray::iterator BodiesArray::begin()
{
    return std::begin(m_bodies);
}

BodiesArray::iterator BodiesArray::end()
{
    return std::end(m_bodies);
}

BodiesArray::const_iterator BodiesArray::begin() const
{
    return std::begin(m_bodies);
}

BodiesArray::const_iterator BodiesArray::end() const
{
    return std::end(m_bodies);
}