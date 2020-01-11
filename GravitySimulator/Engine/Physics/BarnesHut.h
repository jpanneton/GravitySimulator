#pragma once

#include "BodiesArray.h"
#include "Engine/Core/CopyableAtomic.h"
#include "Engine/Core/FreeList.h"
#include <glm/glm.hpp>
#include <array>
#include <memory>
#include <optional>

class BarnesHutOctree
{
    static constexpr int32_t DIM = 1 << 3; // 3D = 8 octants

public:
    using Collision = std::pair<int32_t, int32_t>;
    using CollisionContainer = std::vector<Collision>;

    struct BoundingBox
    {
        bool contains(const glm::vec3& point) const;

        glm::vec3 center;
        float radius = {};
    };

    struct OctreeElement
    {
        // Gravity simulation
        glm::vec3 position;
        float mass = {};

        // Collision detection
        float radius = {}; // Bounding sphere radius of children or the body itself
        CopyableAtomic<int32_t> index = -1; // Index of the body in the original body array
    };
    
    struct OctreeNode
    {
        // If this node is a branch, stores the index of its first child
        // If this node is a leaf and contains a body, stores -1
        // If this node is a leaf and is empty, stores -2
        int32_t firstChild = -2;

        // Data of the node
        OctreeElement data;
        BoundingBox box;

        bool isLeafNode() const noexcept
        {
            return firstChild < 0;
        }

        bool isEmptyLeafNode() const noexcept
        {
            return isLeafNode() && firstChild == -2;
        }
    };

    struct OctreeNodeGroup
    {
        OctreeNodeGroup() = default;
        OctreeNodeGroup(const BoundingBox& parentBox)
        {
            for (int32_t i = 0; i < DIM; ++i)
            {
                octants[i].box = getChildBoxFromOctant(parentBox, i);
            }
        }

        std::array<OctreeNode, DIM> octants;
    };

    using Ptr = std::unique_ptr<BarnesHutOctree>;

    BarnesHutOctree() = default;
    void reserve(size_t capacity);

private:
    // Determines which region of the tree would contain the point
    // Children follow a predictable pattern to make accesses easier (Morton code)
    // - means less than origin, + means greater than origin
    // child:  0 1 2 3 4 5 6 7
    // x:      - - - - + + + +
    // y:      - - + + - - + +
    // z:      - + - + - + - +
    static int getOctantContainingPoint(const BoundingBox& box, const glm::vec3& point);
    static BoundingBox getChildBoxFromOctant(const BoundingBox& parentBox, int32_t regionIndex);

    void updateWorldBounds(const BodiesArray& bodies);
    void insert(OctreeNode& currentNode, const OctreeElement& element);
    void updateTree(OctreeNode& currentNode); // Updates the center of mass of parent nodes from child nodes
    glm::vec3 calculateForce(const OctreeNode& currentNode, const Body& body, scalar gravityFactor) const;
    int32_t detectCollision(OctreeNode& currentNode, const Body& body, int32_t bodyIndex);

public:
    void buildTree(const BodiesArray& bodies);
    glm::vec3 calculateForce(const Body& body, scalar gravityFactor) const;
    // Returns only the first collision encountered (a body can only collide with another one each update)
    // Since the system is being updated frequently, multi-collisions are handled over multiple updates
    int32_t detectCollision(const Body& body, int32_t bodyIndex);
    
private:
    OctreeNode m_root; // Root node
    FreeList<OctreeNodeGroup> m_nodes; // Other nodes
};