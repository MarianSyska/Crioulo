#ifndef CRIOULO_H 
#define CRIOULO_H

#include <vector>
#include <memory>
#include <GLFW/glfw3.h>
#include "camera.h"
#include "material.h"
#include "mesh_instance.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "light.h"

namespace Crioulo 
{
    class Renderer
    {
        public:
            Renderer(GLFWwindow* window);
            ~Renderer(){}
            void drawScene();
            
            std::shared_ptr<Mesh> loadMesh(const MeshData& data);
            std::shared_ptr<Texture> loadTexture(const TextureData& data);
            std::shared_ptr<Shader> loadShader(const char* vertexCode, const char* fragmentCode);
            
            void registerMeshInstance(const std::shared_ptr<MeshInstance>& instance);
            void registerPointLight(const std::shared_ptr<PointLight>& pointLight);

            void setActiveCamera(const std::shared_ptr<Camera>& camera);

            inline void setWindowHeight(const unsigned int& windowHeight){
                m_windowHeight = windowHeight;
            }

            inline void setWindowWidth(const unsigned int& windowWidth){
                m_windowWidth = windowWidth;
            }

        private:
            GLFWwindow* window;
            unsigned int m_windowHeight, m_windowWidth;
            
            std::vector<std::shared_ptr<MeshInstance>> m_instances;
            
            std::shared_ptr<Camera> m_activeCamera;

            std::vector<std::shared_ptr<PointLight>> m_pointLights;
            
            void bindOpenGLFunctions();
    };
}
#endif