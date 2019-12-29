#pragma once

#include "BodiesArray.h"
#include <glm/glm.hpp>
#include <array>
#include <memory>
#include <optional>

class BarnesHutOctree
{
public:
    class BoundingBox
    {
    public:
        BoundingBox(const glm::vec3& center, float radius);
        const glm::vec3& getCenter() const;
        float getRadius() const;
        bool contains(const glm::vec3& point) const;

    private:
        glm::vec3 m_center;
        float m_radius;
    };

    class BarnesHutBody
    {
    public:
        using Ptr = std::unique_ptr<BarnesHutBody>;

        BarnesHutBody(const glm::vec3& position, float mass);
        const glm::vec3& getPosition() const;
        float getMass() const;

    private:
        glm::vec3 m_position;
        float m_mass;
    };

    using Ptr = std::unique_ptr<BarnesHutOctree>;

    BarnesHutOctree(const BoundingBox& box);

private:
    // Determines which region of the tree would contain the point
    // Children follow a predictable pattern to make accesses easier (Morton code)
    // - means less than origin, + means greater than origin
    // child:  0 1 2 3 4 5 6 7
    // x:      - - - - + + + +
    // y:      - - + + - - + +
    // z:      - + - + - + - +
    int getOctantContainingPoint(const glm::vec3& point) const;
    BoundingBox getChildBoxFromRegion(int32_t regionIndex);

    bool isLeafNode() const;
    void insert(const BarnesHutBody& body);
    void updateTree(); // Updates the center of mass of parent nodes from child nodes

public:
    void buildTree(const BodiesArray& bodies);
    glm::vec3 calculateForce(const Body& body, scalar gravityFactor);
    
private:
    BoundingBox m_box; // Bounding box of the node
    std::array<Ptr, 8> m_children; // Octants of the node
    std::optional<BarnesHutBody> m_data; // Data of the node
};