#ifndef CRIOULO_CAMERA_H
#define CRIOULO_CAMERA_H

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace Crioulo 
{
    struct Camera
    {   
            glm::vec3 position;
            glm::vec3 front;
            glm::vec3 up;
            glm::vec3 right;
            
            float zoom;
            float nearPlane;
            float farPlane;

            inline glm::mat4 getViewTransformationMatrix() {
                return glm::lookAt(position, position + front, up);
            }

            inline glm::mat4 getProjectionTransformationMatrix(float surfaceRatio) {
                return glm::perspective(glm::radians(zoom), surfaceRatio, nearPlane, farPlane);
            }
    };
};
#endif