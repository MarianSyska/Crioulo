#ifndef CRIOULO_PRIMITIVE_SHAPES_H
#define CRIOULO_PRIMITIVE_SHAPES_H

#include <array>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>


namespace Crioulo::PrimitiveShapes {

    template<typename VerticeType, size_t verticesCount, size_t indicesCount>
    struct Shape {
        std::array<VerticeType, verticesCount> vertices;
        std::array<uint32_t, indicesCount> indices;
    };

    using CubeShape = Shape<glm::vec3, 8, 36>;
    using PlaneShape = Shape<glm::vec2, 4, 6>;

    constexpr CubeShape cube(float width, float height, float length) {
        std::array<glm::vec3, 8> cubeVertices{{
                // Front face
                { -0.5f,  0.5f,  0.5f }, // 0: Top-left
                { -0.5f, -0.5f,  0.5f }, // 1: Bottom-left
                {  0.5f, -0.5f,  0.5f }, // 2: Bottom-right
                {  0.5f,  0.5f,  0.5f }, // 3: Top-right

                // Back face
                { -0.5f,  0.5f, -0.5f }, // 4: Top-left
                { -0.5f, -0.5f, -0.5f }, // 5: Bottom-left
                {  0.5f, -0.5f, -0.5f }, // 6: Bottom-right
                {  0.5f,  0.5f, -0.5f }  // 7: Top-right
        }};

       for (std::size_t i = 0; i < cubeVertices.size(); ++i) {
           cubeVertices[i].x *= width;
           cubeVertices[i].y *= height;
           cubeVertices[i].z *= length;
       }

       constexpr std::array<uint32_t, 36> cubeIndices{
               // Front Face
               0, 2, 1,  0, 3, 2,
               // Back Face
               4, 5, 6,  4, 6, 7,
               // Left Face
               4, 1, 5,  4, 0, 1,
               // Right Face
               3, 6, 2,  3, 7, 6,
               // Top Face
               4, 3, 0,  4, 7, 3,
               // Bottom Face
               1, 6, 5,  1, 2, 6
       };

       return { cubeVertices, cubeIndices };
	}

    constexpr CubeShape cube(float extend) {
        return cube(extend, extend, extend);
    }

    constexpr PlaneShape plane(float width, float height) {
        std::array<glm::vec2, 4> planeVertices{{
                { -0.5f,  0.5f },
                { -0.5f, -0.5f },
                {  0.5f, -0.5f },
                {  0.5f,  0.5f }
        }};

        for (std::size_t i = 0; i < planeVertices.size(); ++i) {
            planeVertices[i].x *= width;
            planeVertices[i].y *= height;
        }

        constexpr std::array<uint32_t, 6> planeIndices{
            0, 2, 1,  0, 3, 2
        };

        return { planeVertices, planeIndices };
    }

    constexpr PlaneShape plane(float extend) {
        return plane(extend, extend);
    }

    constexpr CubeShape UNIT_CUBE = cube(1.0f);
    constexpr PlaneShape UNIT_PLANE = plane(1.0f);
}
	
#endif

