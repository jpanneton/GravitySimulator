#include "BarnesHut.h"

namespace
{
    template<typename T>
    constexpr T calculateGravitationalForce(T gravityFactor, T mass, T distanceSquared)
    {
        static_assert(std::is_floating_point_v<T>);
        return gravityFactor * (mass / distanceSquared);
    }
}

BarnesHutOctree::BoundingBox::BoundingBox(const glm::vec3& center, float radius)
    : m_center(center)
    , m_radius(radius)
{
}

const glm::vec3& BarnesHutOctree::BoundingBox::getCenter() const
{
    return m_center;
}

float BarnesHutOctree::BoundingBox::getRadius() const
{
    return m_radius;
}

bool BarnesHutOctree::BoundingBox::contains(const glm::vec3& point) const
{
    const glm::vec3 delta = glm::abs(point - m_center);
    return m_radius >= delta.x && m_radius >= delta.y && m_radius >= delta.z;
}

//------------------------------------------------------------------------

BarnesHutOctree::BarnesHutBody::BarnesHutBody(const glm::vec3& position, float mass)
    : m_position(position)
    , m_mass(mass)
{
}

const glm::vec3& BarnesHutOctree::BarnesHutBody::getPosition() const
{
    return m_position;
}

float BarnesHutOctree::BarnesHutBody::getMass() const
{
    return m_mass;
}

//------------------------------------------------------------------------

BarnesHutOctree::BarnesHutOctree(const BoundingBox& box)
    : m_box(box)
{
}

int BarnesHutOctree::getOctantContainingPoint(const glm::vec3& point) const
{
    int code = 0;
    if (point.x >= m_box.getCenter().x) code |= 0b0100;
    if (point.y >= m_box.getCenter().y) code |= 0b0010;
    if (point.z >= m_box.getCenter().z) code |= 0b0001;
    return code;
}

BarnesHutOctree::BoundingBox BarnesHutOctree::getChildBoxFromRegion(int32_t regionIndex)
{
    glm::vec3 newOrigin = m_box.getCenter();
    newOrigin.x += (regionIndex & 0b0100 ? 0.5f : -0.5f) * m_box.getRadius();
    newOrigin.y += (regionIndex & 0b0010 ? 0.5f : -0.5f) * m_box.getRadius();
    newOrigin.z += (regionIndex & 0b0001 ? 0.5f : -0.5f) * m_box.getRadius();
    return { newOrigin, 0.5f * m_box.getRadius() };
}

bool BarnesHutOctree::isLeafNode() const
{
    return m_children[0] == nullptr;
}

void BarnesHutOctree::insert(const BarnesHutBody& body)
{
    assert(m_box.contains(body.getPosition()));

    if (isLeafNode())
    {
        // Empty leaf
        if (!m_data)
        {
            m_data = body;
        }
        // Occupied leaf (needs to be split up)
        else
        {
            // Save for later re-insertion
            const auto oldBody = *m_data;
            m_data = {};

            // Split the current node
            for (int32_t i = 0; i < m_children.size(); ++i)
            {
                // Compute new bounding box for this child
                m_children[i] = std::make_unique<BarnesHutOctree>(getChildBoxFromRegion(i));
            }

            // Re-insert the old body and insert the new one
            const int oldPointOctant = getOctantContainingPoint(oldBody.getPosition());
            const int newPointOctant = getOctantContainingPoint(body.getPosition());
            m_children[oldPointOctant]->insert(oldBody);
            m_children[newPointOctant]->insert(body);
        }
    }
    else
    {
        // Find leaf in which to insert the new body recursively
        const int octant = getOctantContainingPoint(body.getPosition());
        m_children[octant]->insert(body);
    }
}

void BarnesHutOctree::updateTree()
{
    // Data is already up-to-date
    if (isLeafNode())
        return;

    // Back propagate children first
    for (size_t i = 0; i < m_children.size(); ++i)
    {
        if (!m_children[i]->isLeafNode())
        {
            m_children[i]->updateTree();
        }
    }

    // Calculate total mass and weighted average center of mass
    float totalMass = {};
    glm::vec3 averageCenter;
    for (size_t i = 0; i < m_children.size(); ++i)
    {
        if (m_children[i]->m_data)
        {
            totalMass += m_children[i]->m_data->getMass();
            averageCenter += m_children[i]->m_data->getMass() * m_children[i]->m_data->getPosition();
        }
    }
    averageCenter /= totalMass;

    m_data = { averageCenter, totalMass };
}

void BarnesHutOctree::buildTree(const BodiesArray& bodies)
{
    for (const Body& body : bodies)
    {
        insert({ body.position(), body.mass() });
    }
    updateTree();
}

glm::vec3 BarnesHutOctree::calculateForce(const Body& body, scalar gravityFactor)
{
    if (!m_data)
    {
        assert(isLeafNode());
        return {};
    }

    const glm::vec3 gravityVector = m_data->getPosition() - body.position();
    const float centerOfMassDistance = glm::length(gravityVector);

    if (isLeafNode())
    {
        // If the current node is the body for which we are calculating force (itself)
        if (centerOfMassDistance == 0.0f)
            return {};

        const float gravitationalForce = calculateGravitationalForce(gravityFactor, m_data->getMass(), centerOfMassDistance * centerOfMassDistance);
        return gravitationalForce * glm::normalize(gravityVector);
    }
    else
    {
        constexpr float theta = 1.0f; // Approximation level (0 = no approximation)
        static_assert(theta >= 0.0f && theta <= 2.0f);

        float octantSize = 2 * m_box.getRadius();
        if (octantSize / centerOfMassDistance < theta)
        {
            const float gravitationalForce = calculateGravitationalForce(gravityFactor, m_data->getMass(), centerOfMassDistance * centerOfMassDistance);
            return gravitationalForce * glm::normalize(gravityVector);
        }
        else
        {
            glm::vec3 force;
            for (size_t i = 0; i < m_children.size(); ++i)
            {
                force += m_children[i]->calculateForce(body, gravityFactor);
            }
            return force;
        }
    }
}