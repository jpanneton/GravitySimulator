#pragma once

#include "BodiesArray.h"
#include "PhysicsEngine.h"
#include "Serializer.h"
#include <SFML/System/Time.hpp>
#include <vector>

class System
{
public:
    using iterator = BodiesArray::iterator;
    using collision = std::pair<iterator, iterator>;

public:
    System() = default;
    System(const PhysicsEngine& engine);
    System(const BodiesArray& bodies, const PhysicsEngine& engine, scalar timescale = 1.0f);
    System(const System& other);

    iterator begin();
    iterator end();

    void update(sf::Time dt);
    void addBody(const Body& body);

    scalar timescale() const;
    void increaseTimescale();
    void decreaseTimescale();

    void save(std::ostream& os);

private:
    void applyGravity(float timespan);
    void moveAllBodies(float timespan);
    void detectCollisions();
    void resolveCollisions();

private:
    BodiesArray m_bodies;
    PhysicsEngine m_physicsEngine;
    scalar m_timescale;
    scalar m_timestep;

    std::vector<collision> m_collisions;
};