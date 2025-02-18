#ifndef CRIOULO_LIGHT_H
#define CRIOULO_LIGHT_H
#include <glm/vec3.hpp>

namespace Crioulo 
{
    class Renderer;

    class PointLight
    {
        friend class Renderer;

        public:

            PointLight()
            {
                
            }

            void setPosition(const glm::vec3& p_position)
            {
                m_position = p_position;
            }

            void setIntensity(float p_intensity)
            {
                m_intensity = p_intensity;
            }
            
            
            void setColor(const glm::vec3& p_color)
            {
                m_color = p_color;
            }

        private:
            glm::vec3 m_position;
            float m_intensity;
            glm::vec3 m_color;
    };
}
#endif