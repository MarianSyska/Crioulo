#ifndef CRIOULO_CAMERA_H
#define CRIOULO_CAMERA_H

#include <glm/vec3.hpp>

namespace Crioulo 
{
    class Renderer;

    class Camera
    {   
        friend class Renderer; 

        public:

        inline void setPosition(const glm::vec3& position)
        {
            m_position = position;
        }

        inline void setFront(const glm::vec3& front)
        {
            m_front = front;
        }

        inline void setUp(const glm::vec3& up)
        {
            m_up = up;
        }

        inline void setRight(const glm::vec3& right)
        {
            m_right = right;
        }

        inline void setZoom(const float& zoom)
        {
            m_zoom = zoom;
        }

        inline void setNearPlane(const float& nearPlane)
        {
            m_nearPlane = nearPlane;
        }

        inline void setFarPlane(const float& farPlane)
        {
            m_farPlane = farPlane;
        }

        private:

            glm::vec3 m_position;
            glm::vec3 m_front;
            glm::vec3 m_up;
            glm::vec3 m_right;
            
            float m_zoom;
            float m_nearPlane;
            float m_farPlane;
    };
};
#endif