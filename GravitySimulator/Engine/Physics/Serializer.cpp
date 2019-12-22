#include "Serializer.h"
#include <algorithm>
#include <iterator>

std::ostream& operator<<(std::ostream& os, const vec3& vec)
{
    return os << vec.x << ' ' << vec.y << ' ' << vec.z;
}

std::istream& operator>>(std::istream& is, vec3& vec)
{
    if (!is) return is;
    scalar x, y, z;
    if (is >> x >> y >> z)
        vec = vec3{ x, y, z };
    return is;
}

std::ostream& operator<<(std::ostream& os, const Body& body)
{
    return os << body.position() << ' ' << body.velocity() << ' ' << body.mass() << ' ' << body.material();
}

std::istream& operator>>(std::istream& is, Body& body)
{
    if (!is) return is;
    vec3 position, velocity;
    scalar mass;
    int material;
    if (is >> position >> velocity >> mass >> material)
        body = Body{ position, velocity, mass , Material(material) };
    return is;
}

void serializeBodies(std::ostream& os, const BodiesArray& bodies)
{
    std::copy(
        bodies.begin(), 
        bodies.end(), 
        std::ostream_iterator<Body>(os, "\n"));
}

void deserializeBodies(std::istream& is, BodiesArray& bodies)
{
    if (!is) return;
    std::copy(
        std::istream_iterator<Body>(is),
        std::istream_iterator<Body>(),
        std::back_inserter(bodies));
}