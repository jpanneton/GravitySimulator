#include "MeshGeneration.h"

namespace MeshGeneration
{	

    const float rad = 1.0f;		
    const float meridian = 20.0f;
    const float latitude = 20.0f;
    const float uv_size = 1.f / 20.0f;
        
    std::shared_ptr<Mesh> generateCube(std::vector<Texture> textures)
    {
        std::vector<Vertex> vertices = {
            // { { Position }, { Normal }, { TexCoord } }
            { { -0.5f, -0.5f, -0.5f },{},{ 0.0f, 0.0f } },
            { { 0.5f, -0.5f, -0.5f },{},{ 1.0f, 0.0f } },
            { { 0.5f,  0.5f, -0.5f },{},{ 1.0f, 1.0f } },
            { { 0.5f,  0.5f, -0.5f },{},{ 1.0f, 1.0f } },
            { { -0.5f,  0.5f, -0.5f },{},{ 0.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f },{},{ 0.0f, 0.0f } },

            { { -0.5f, -0.5f,  0.5f },{},{ 0.0f, 0.0f } },
            { { 0.5f, -0.5f,  0.5f },{},{ 1.0f, 0.0f } },
            { { 0.5f,  0.5f,  0.5f },{},{ 1.0f, 1.0f } },
            { { 0.5f,  0.5f,  0.5f },{},{ 1.0f, 1.0f } },
            { { -0.5f, 0.5f,  0.5f },{},{ 0.0f, 1.0f } },
            { { -0.5f, -0.5f,  0.5f },{},{ 0.0f, 0.0f } },

            { { -0.5f,  0.5f,  0.5f },{},{ 1.0f, 0.0f } },
            { { -0.5f,  0.5f, -0.5f },{},{ 1.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f },{},{ 0.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f },{},{ 0.0f, 1.0f } },
            { { -0.5f, -0.5f,  0.5f },{},{ 0.0f, 0.0f } },
            { { -0.5f,  0.5f,  0.5f },{},{ 1.0f, 0.0f } },

            { { 0.5f,  0.5f,  0.5f },{},{ 1.0f, 0.0f } },
            { { 0.5f,  0.5f, -0.5f },{},{ 1.0f, 1.0f } },
            { { 0.5f, -0.5f, -0.5f },{},{ 0.0f, 1.0f } },
            { { 0.5f, -0.5f, -0.5f },{},{ 0.0f, 1.0f } },
            { { 0.5f, -0.5f,  0.5f },{},{ 0.0f, 0.0f } },
            { { 0.5f,  0.5f,  0.5f },{},{ 1.0f, 0.0f } },

            { { -0.5f, -0.5f, -0.5f },{},{ 0.0f, 1.0f } },
            { { 0.5f, -0.5f, -0.5f },{},{ 1.0f, 1.0f } },
            { { 0.5f, -0.5f,  0.5f },{},{ 1.0f, 0.0f } },
            { { 0.5f, -0.5f,  0.5f },{},{ 1.0f, 0.0f } },
            { { -0.5f, -0.5f,  0.5f },{},{ 0.0f, 0.0f } },
            { { -0.5f, -0.5f, -0.5f },{},{ 0.0f, 1.0f } },

            { { -0.5f,  0.5f, -0.5f },{},{ 0.0f, 1.0f } },
            { { 0.5f,  0.5f, -0.5f },{},{ 1.0f, 1.0f } },
            { { 0.5f,  0.5f,  0.5f },{},{ 1.0f, 0.0f } },
            { { 0.5f,  0.5f,  0.5f },{},{ 1.0f, 0.0f } },
            { { -0.5f,  0.5f,  0.5f },{},{ 0.0f, 0.0f } },
            { { -0.5f,  0.5f, -0.5f },{},{ 0.0f, 1.0f } }
        };

        return std::make_shared<Mesh>(vertices, textures);
    }

    std::shared_ptr<Mesh> generateSkyBox(std::vector<Texture> textures)
    {
        std::vector<Vertex> vertices = {
            // { { Position }, { Normal }, { TexCoord } }
            { { -1.0f,  1.0f, -1.0f },{},{} },
            { { -1.0f, -1.0f, -1.0f },{},{} },
            { {  1.0f, -1.0f, -1.0f },{},{} },
            { {  1.0f, -1.0f, -1.0f },{},{} },
            { {  1.0f,  1.0f, -1.0f },{},{} },
            { { -1.0f,  1.0f, -1.0f },{},{} },

            { { -1.0f, -1.0f,  1.0f },{},{} },
            { { -1.0f, -1.0f, -1.0f },{},{} },
            { { -1.0f,  1.0f, -1.0f },{},{} },
            { { -1.0f,  1.0f, -1.0f },{},{} },
            { { -1.0f,  1.0f,  1.0f },{},{} },
            { { -1.0f, -1.0f,  1.0f },{},{} },

            { {  1.0f, -1.0f, -1.0f },{},{} },
            { {  1.0f, -1.0f,  1.0f },{},{} },
            { {  1.0f,  1.0f,  1.0f },{},{} },
            { {  1.0f,  1.0f,  1.0f },{},{} },
            { {  1.0f,  1.0f, -1.0f },{},{} },
            { {  1.0f, -1.0f, -1.0f },{},{} },

            { { -1.0f, -1.0f,  1.0f },{},{} },
            { { -1.0f,  1.0f,  1.0f },{},{} },
            { {  1.0f,  1.0f,  1.0f },{},{} },
            { {  1.0f,  1.0f,  1.0f },{},{} },
            { {  1.0f, -1.0f,  1.0f },{},{} },
            { { -1.0f, -1.0f,  1.0f },{},{} },

            { { -1.0f,  1.0f, -1.0f },{},{} },
            { {  1.0f,  1.0f, -1.0f },{},{} },
            { {  1.0f,  1.0f,  1.0f },{},{} },
            { {  1.0f,  1.0f,  1.0f },{},{} },
            { { -1.0f,  1.0f,  1.0f },{},{} },
            { { -1.0f,  1.0f, -1.0f },{},{} },

            { { -1.0f, -1.0f, -1.0f },{},{} },
            { { -1.0f, -1.0f,  1.0f },{},{} },
            { {  1.0f, -1.0f, -1.0f },{},{} },
            { {  1.0f, -1.0f, -1.0f },{},{} },
            { { -1.0f, -1.0f,  1.0f },{},{} },
            { {  1.0f, -1.0f,  1.0f },{},{} }
        };

        return std::make_shared<Mesh>(vertices, textures);
    }


    std::shared_ptr<Mesh> generateSphere(std::vector<Texture> textures)
    {
        std::vector<Vertex> vertices{};
        float phi_stack = 0.f;
        float theta_slice = 0.f;
        float phi_stack_size = glm::pi<float>() / latitude;
        float theta_slice_size = glm::two_pi<float>() / meridian;

        for (int current_stack = 0; current_stack < latitude; current_stack++)
        {
            float v_coord = (uv_size * current_stack);
            for (int current_slice = 0; current_slice < meridian; current_slice++)
            {
                float u_coord = (uv_size * current_slice);

                //Points
                float x1 = rad*sin(phi_stack)*cos(theta_slice);
                float y1 = rad*sin(phi_stack)*sin(theta_slice);
                float z1 = rad*cos(phi_stack);

                float x2 = rad*sin(phi_stack + phi_stack_size)*cos(theta_slice);
                float y2 = rad*sin(phi_stack + phi_stack_size)*sin(theta_slice);
                float z2 = rad*cos(phi_stack + phi_stack_size);

                float x3 = rad*sin(phi_stack + phi_stack_size)*cos(theta_slice + theta_slice_size);
                float y3 = rad*sin(phi_stack + phi_stack_size)*sin(theta_slice + theta_slice_size);
                float z3 = rad*cos(phi_stack + phi_stack_size);

                float x4 = rad*sin(phi_stack)*cos(theta_slice + theta_slice_size);
                float y4 = rad*sin(phi_stack)*sin(theta_slice + theta_slice_size);
                float z4 = rad*cos(phi_stack);

                //First
                vertices.push_back({ { x1, y1, z1 },{ x1*3.4, y1*3.4, z1*3.4 },{ u_coord, v_coord } });
                
                vertices.push_back({ { x3, y3, z3 },{ x3*3.4, y3*3.4, z3*3.4 },{ u_coord + uv_size, v_coord + uv_size } });

                vertices.push_back({ { x4, y4, z4 },{ x4*3.4, y4*3.4, z4*3.4 },{ u_coord + uv_size, v_coord } });

                //Second			
                vertices.push_back({ { x1, y1, z1 },{ x1*3.4, y1*3.4, z1*3.4 },{ u_coord, v_coord } });

                vertices.push_back({ { x3, y3, z3 },{ x3*3.4, y3*3.4, z3*3.4 },{ u_coord + uv_size, v_coord + uv_size } });

                vertices.push_back({ { x2, y2, z2 },{ x2*3.4, y2*3.4, z2*3.4 },{ u_coord, v_coord + uv_size } });

                theta_slice += theta_slice_size;
            }
            phi_stack += phi_stack_size;
        }

        
        return std::make_shared<Mesh>(vertices, textures);
    }

}