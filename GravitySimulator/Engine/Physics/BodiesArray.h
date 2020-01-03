#pragma once

#include "Body.h"
#include <vector>

class BodiesArray
{
public:
    static const size_t MAX_BODIES = 20'000;

    using value_type = Body;
    using container = std::vector<value_type>;
    using iterator = container::iterator;
    using const_iterator = container::const_iterator;

public:
    BodiesArray();

    void push_back(const Body& body);
    void merge(iterator target, iterator source);
    size_t size() const;

    Body& operator[](int32_t n);
    const Body& operator[](int32_t n) const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

private:
    container m_bodies;
};
