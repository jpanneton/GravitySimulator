#include "BarnesHut.h"
#include <glm/gtx/component_wise.hpp>

namespace
{
    template<typename T, class Func>
    inline T roundToPowerOfTwo(T value, Func roundingFunction)
    {
        const T sign = value < T(0) ? T(-1) : T(1);
        value = std::abs(value);
        return sign * (value > T(2) ?
            std::exp2(roundingFunction(std::log2(value))) :
            roundingFunction(value));
    }

    template<typename T>
    constexpr T calculateGravitationalForce(T gravityFactor, T mass, T distanceSquared)
    {
        static_assert(std::is_floating_point_v<T>);
        return gravityFactor * (mass / distanceSquared);
    }
}

bool BarnesHutOctree::BoundingBox::contains(const glm::vec3& point) const
{
    const glm::vec3 delta = glm::abs(point - center);
    return radius >= delta.x && radius >= delta.y && radius >= delta.z;
}

//------------------------------------------------------------------------

void BarnesHutOctree::reserve(size_t capacity)
{
    const size_t minTreeHeight = static_cast<size_t>(std::ceil(std::log(capacity) / std::log(DIM)));
    // Finite geometric series
    const size_t maxNodeCount = static_cast<size_t>(std::ceil((std::pow(DIM, minTreeHeight + 1) - 1) / float(DIM - 1)));
    // Remove the root from the count (-1)
    m_nodes.reserve(maxNodeCount - 1);
}

int BarnesHutOctree::getOctantContainingPoint(const BoundingBox& box, const glm::vec3& point)
{
    int code = 0;
    if (point.x >= box.center.x) code |= 0b0100;
    if (point.y >= box.center.y) code |= 0b0010;
    if (point.z >= box.center.z) code |= 0b0001;
    return code;
}

BarnesHutOctree::BoundingBox BarnesHutOctree::getChildBoxFromOctant(const BoundingBox& parentBox, int32_t regionIndex)
{
    glm::vec3 newOrigin = parentBox.center;
    newOrigin.x += (regionIndex & 0b0100 ? 0.5f : -0.5f) * parentBox.radius;
    newOrigin.y += (regionIndex & 0b0010 ? 0.5f : -0.5f) * parentBox.radius;
    newOrigin.z += (regionIndex & 0b0001 ? 0.5f : -0.5f) * parentBox.radius ;
    return { newOrigin, 0.5f * parentBox.radius };
}

void BarnesHutOctree::updateWorldBounds(const BodiesArray& bodies)
{
    // Calculate world bounds
    glm::vec3 minWorldPoint(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    glm::vec3 maxWorldPoint(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

    for (const Body& body : bodies)
    {
        const glm::vec3& bodyPosition = body.position();
        if (bodyPosition.x < minWorldPoint.x) minWorldPoint.x = bodyPosition.x;
        if (bodyPosition.x > maxWorldPoint.x) maxWorldPoint.x = bodyPosition.x;
        if (bodyPosition.y < minWorldPoint.y) minWorldPoint.y = bodyPosition.y;
        if (bodyPosition.y > maxWorldPoint.y) maxWorldPoint.y = bodyPosition.y;
        if (bodyPosition.z < minWorldPoint.z) minWorldPoint.z = bodyPosition.z;
        if (bodyPosition.z > maxWorldPoint.z) maxWorldPoint.z = bodyPosition.z;
    }

    const glm::vec3 worldRadius = 0.5f * (maxWorldPoint - minWorldPoint);
    const glm::vec3 worldCenter = minWorldPoint + worldRadius;

    // Round world center coordinates to closest powers of 2
    const glm::vec3 newWorldCenter = { roundToPowerOfTwo(worldCenter.x, std::roundf), roundToPowerOfTwo(worldCenter.y, std::roundf), roundToPowerOfTwo(worldCenter.z, std::roundf) };
    // Ceil world radius to closest power of 2 while taking into account the new center offset
    const float newWorldRadius = roundToPowerOfTwo(glm::compMax(worldRadius + glm::abs(newWorldCenter - worldCenter)), std::ceilf);
    // Since bounds are powers of 2, floating-point errors are avoided when dividing the space
    m_root.box = { newWorldCenter, newWorldRadius };
}

void BarnesHutOctree::insert(OctreeNode& currentNode, const OctreeElement& element)
{
    assert(currentNode.box.contains(element.position));

    if (currentNode.isLeafNode())
    {
        // Empty leaf
        if (currentNode.isEmptyLeafNode())
        {
            currentNode.firstChild = -1;
            currentNode.data = element;
        }
        // Occupied leaf (needs to be split up)
        else
        {
            // Save current node data for later re-insertion
            const auto oldElement = currentNode.data;

            // No need to reinitialize the data since we update parents later (in updateTree)
            // currentNode.data = {};
            
            // Memory must be properly pre-allocated
            // Otherwise, currentNode will be invalidated if m_nodes grows!
            assert(m_nodes.size() < m_nodes.capacity());
            
            // Update current node references
            const auto newNodeIndex = m_nodes.insert({ currentNode.box });
            currentNode.firstChild = newNodeIndex;
            
            const int32_t oldPointOctant = getOctantContainingPoint(currentNode.box, oldElement.position);
            const int32_t newPointOctant = getOctantContainingPoint(currentNode.box, element.position);
            insert(m_nodes[newNodeIndex].octants[oldPointOctant], oldElement);
            insert(m_nodes[newNodeIndex].octants[newPointOctant], element);
        }
    }
    else
    {
        // Find leaf in which to insert the new body recursively
        const int32_t octantIndex = getOctantContainingPoint(currentNode.box, element.position);
        insert(m_nodes[currentNode.firstChild].octants[octantIndex], element);
    }
}

void BarnesHutOctree::updateTree(OctreeNode& currentNode)
{
    // Data is already up-to-date
    if (currentNode.isLeafNode())
        return;

    // Back propagate children first
    for (int32_t i = 0; i < DIM; ++i)
    {
        OctreeNode& childNode = m_nodes[currentNode.firstChild].octants[i];
        if (!childNode.isLeafNode())
        {
            updateTree(childNode);
        }
    }

    // Calculate total mass and weighted average center of mass
    float totalMass = {};
    glm::vec3 averageCenter;
    for (int32_t i = 0; i < DIM; ++i)
    {
        const OctreeNode& childNode = m_nodes[currentNode.firstChild].octants[i];
        if (!childNode.isEmptyLeafNode())
        {
            totalMass += childNode.data.mass;
            averageCenter += childNode.data.mass * childNode.data.position;
        }
    }
    averageCenter /= totalMass;

    // Calculate bounding radius
    float boundingRadius = {};
    for (int32_t i = 0; i < DIM; ++i)
    {
        const OctreeNode& childNode = m_nodes[currentNode.firstChild].octants[i];
        if (!childNode.isEmptyLeafNode())
        {
            const float maxChildDistance = glm::distance(averageCenter, childNode.data.position) + childNode.data.radius;
            if (maxChildDistance > boundingRadius)
            {
                boundingRadius = maxChildDistance;
            }
        }
    }

    currentNode.data = { averageCenter, totalMass, boundingRadius };
}

glm::vec3 BarnesHutOctree::calculateForce(const OctreeNode& currentNode, const Body& body, scalar gravityFactor) const
{
    if (currentNode.isEmptyLeafNode())
        return {};

    assert(currentNode.isLeafNode() || currentNode.firstChild != -2);

    const glm::vec3 gravityVector = currentNode.data.position - body.position();
    const float centerOfMassDistance = glm::length(gravityVector);

    if (currentNode.isLeafNode())
    {
        // If the current node is the body for which we are calculating force (itself)
        if (centerOfMassDistance == 0.0f)
            return {};

        const float gravitationalForce = calculateGravitationalForce(gravityFactor, currentNode.data.mass, centerOfMassDistance * centerOfMassDistance);
        return gravitationalForce * glm::normalize(gravityVector);
    }
    else
    {
        constexpr float theta = 1.0f; // Approximation level (0 = no approximation)
        static_assert(theta >= 0.0f && theta <= 2.0f);

        float octantSize = 2 * currentNode.box.radius;
        if (octantSize / centerOfMassDistance < theta)
        {
            const float gravitationalForce = calculateGravitationalForce(gravityFactor, currentNode.data.mass, centerOfMassDistance * centerOfMassDistance);
            return gravitationalForce * glm::normalize(gravityVector);
        }
        else
        {
            glm::vec3 force;
            for (int32_t i = 0; i < DIM; ++i)
            {
                const OctreeNode& childNode = m_nodes[currentNode.firstChild].octants[i];
                force += calculateForce(childNode, body, gravityFactor);
            }
            return force;
        }
    }
}

int32_t BarnesHutOctree::detectCollision(OctreeNode& currentNode, const Body& body, int32_t bodyIndex)
{
    if (!currentNode.isEmptyLeafNode())
    {
        // Collision test
        if (glm::distance(body.position(), currentNode.data.position) <= body.radius() + currentNode.data.radius)
        {
            if (currentNode.isLeafNode())
            {
                // If =, the current node is the body for which we are detecting collision (itself)
                // If <, the current node has already been checked for collisions
                // So, to avoid duplicated entries, we doesn't consider it
                if (currentNode.data.index <= bodyIndex)
                    return -1;
                const int32_t index = currentNode.data.index;
                // Flag the current node as already colliding for future tests
                currentNode.data.index = -1;
                return index;
            }
            else
            {
                for (int32_t i = 0; i < DIM; ++i)
                {
                    OctreeNode& childNode = m_nodes[currentNode.firstChild].octants[i];
                    const int32_t result = detectCollision(childNode, body, bodyIndex);
                    if (result != -1)
                        return result;
                }
            }
        }
    }

    return -1;
}

void BarnesHutOctree::buildTree(const BodiesArray& bodies)
{
    // Reset tree
    m_root = {};
    m_nodes.clear();

    // Update world bounds from data
    updateWorldBounds(bodies);

    // Pre-allocate memory and insert data
    reserve(bodies.size());
    for (int32_t i = 0; i < bodies.size(); ++i)
    {
        const Body& body = bodies[i];
        insert(m_root, { body.position(), body.mass(), body.radius(), i });
    }

    // Update total mass and average position of parents
    updateTree(m_root);
}

glm::vec3 BarnesHutOctree::calculateForce(const Body& body, scalar gravityFactor) const
{
    return calculateForce(m_root, body, gravityFactor);
}

int32_t BarnesHutOctree::detectCollision(const Body& body, int32_t bodyIndex)
{
    return detectCollision(m_root, body, bodyIndex);
}