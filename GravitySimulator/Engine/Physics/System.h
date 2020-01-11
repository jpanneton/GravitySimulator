#pragma once

#include "BarnesHut.h"
#include "BodiesArray.h"
#include "Serializer.h"
#include <SFML/System/Time.hpp>

class System
{
public:
    using iterator = BodiesArray::iterator;

    System() = default;
    System(scalar gravityFactor);
    System(const BodiesArray& bodies, scalar gravityFactor = 1.0f, scalar timescale = 1.0f);
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
    void applyGravity(unsigned int batchIndex, float timespan);
    void moveAllBodies(float timespan);
    void detectCollisions(unsigned int batchIndex);
    void resolveCollisions();

private:
    BodiesArray m_bodies;
    BarnesHutOctree m_octree;
    // Each thread has its own collision container
    std::vector<BarnesHutOctree::CollisionContainer> m_collisionBatches;

    scalar m_gravityFactor = {};
    scalar m_timescale = {};
    scalar m_timestep = {};
};