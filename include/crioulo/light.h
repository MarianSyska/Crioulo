#ifndef CRIOULO_LIGHT_H
#define CRIOULO_LIGHT_H
#include <glm/vec3.hpp>

namespace Crioulo 
{
    class Renderer;

    struct alignas(16) PointLight
    {
        friend class Renderer;

        glm::vec3 position;
        alignas(16) glm::vec3 color;
        alignas(16) float intensity;

    private:
        PointLight() = default;
        ~PointLight() = default;
    };
}
#endif