#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <string>
#include <utility>
#include <algorithm>

#include "camera.h"
#include "material.h"
#include "mesh_instance.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "light.h"

#include "renderer.h"

#ifdef CRIOULO_ENABLE_DEV_DEBUG
#define CRIOULO_GL_ERROR_CHECK \
        switch(glGetError())  \
        { \
            case GL_INVALID_VALUE: \
                std::cout << __LINE__ << " " << __FILE__ << std::endl; \
            case GL_INVALID_ENUM: \
                std::cout << __LINE__ << " " << __FILE__ << std::endl; \
            case GL_INVALID_OPERATION: \
                std::cout << __LINE__ << " " << __FILE__ << std::endl; \
            case GL_INVALID_FRAMEBUFFER_OPERATION: \
                std::cout << __LINE__ << " " << __FILE__ << std::endl; \
            case GL_OUT_OF_MEMORY: \
                std::cout << __LINE__ << " " << __FILE__ << std::endl; \
        }

#else
#define CRIOULO_GL_ERROR_CHECK 
#endif

namespace Crioulo 
{
    void logOpenGL(GLenum source, 
        GLenum type, 
        unsigned int id, 
        GLenum severity, 
        GLsizei length, 
        const char *message, 
        const void *userParam)
    {   
        if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
    }
}
    
using namespace Crioulo;

Renderer::Renderer(GLFWwindow* window)
{
    glfwMakeContextCurrent(window);
    this->window = window;

    bindOpenGLFunctions();

    int flags = 0; 
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(logOpenGL, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glEnable(GL_DEPTH_TEST);
}

void Renderer::drawScene()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(const std::shared_ptr<MeshInstance>& instance : m_instances)
    {          
        std::vector<UniformSlot> uniforms(3 + m_pointLights.size());
        
        glm::mat4 view = glm::lookAt(m_activeCamera->m_position, m_activeCamera->m_position + m_activeCamera->m_front, m_activeCamera->m_up);
        glm::mat4 projection = glm::perspective(glm::radians(m_activeCamera->m_zoom), (float) m_windowWidth / m_windowHeight, m_activeCamera->m_nearPlane, m_activeCamera->m_farPlane);

        uniforms.push_back({"view", UniformType.MAT_FLOAT4, glm::value_ptr(view)});
        uniforms.push_back({"projection", UniformType.MAT_FLOAT4, glm::value_ptr(projection)});
        uniforms.push_back("poitnLightCount", UniformType.UINT, &m_pointLights.size());

        i = 0;
        for(const std::shared_ptr<PointLight>& pointLight : m_pointLights)
        {
            if(i > 3)
            {
                break;
            }
            uniforms.push_back("pointLights[" + std::to_string(i) +  "].position", UniformType.FLOAT3, &pointLight->m_position);
            uniforms.push_back("pointLights[" + std::to_string(i) +  "].intensity", UniformType.FLOAT, &pointLight->m_intensity);
            uniforms.push_back("pointLights[" + std::to_string(i) +  "].color", UniformType.FLOAT4, &pointLight->m_color);
        }
        instance->draw(uniforms);
    }
    glfwSwapBuffers(window);
}

std::shared_ptr<Mesh> Renderer::loadMesh(const MeshData& data)
{
    return std::shared_ptr<Mesh>(new Mesh(data));
} 

std::shared_ptr<Texture> Renderer::loadTexture(const TextureData& data)
{
    return std::shared_ptr<Texture>(new Texture(data));
}

std::shared_ptr<Shader> Renderer::loadShader(const char* vertexCode, const char* fragmentCode)
{
    return std::shared_ptr<Shader>(new Shader(vertexCode, fragmentCode));
}

void Renderer::registerMeshInstance(const std::shared_ptr<MeshInstance>& instance)
{
    m_instances.push_back(instance);
}

void Renderer::registerPointLight(const std::shared_ptr<PointLight>& pointLight)
{
    m_pointLights.push_back(pointLight);
}

void Crioulo::Renderer::setActiveCamera(const std::shared_ptr<Camera> &camera)
{
    m_activeCamera = camera;
}

void Renderer::bindOpenGLFunctions()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
}







