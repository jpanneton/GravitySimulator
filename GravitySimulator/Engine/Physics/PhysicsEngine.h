#pragma once

#include "Body.h"

template <class It, class F>
void for_each_distinct_pair(It i, It e, F fn)
{
    for (; i != e; ++i)
        for (auto j = std::next(i, 1); j != e; ++j)
            fn(*i, *j);
}

class PhysicsEngine
{
public:
    PhysicsEngine();
    PhysicsEngine(scalar g);
    PhysicsEngine(const PhysicsEngine& other);

    void applyGravity(Body& first, Body& second, float timespan) const;
    bool detectCollision(const Body& first, const Body& second) const;

private:
    scalar gravityMagnitude(scalar mass, scalar distance_squared) const;

private:
    scalar G;
};