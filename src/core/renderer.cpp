#include <tracy/Tracy.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <string>
#include <utility>
#include <ranges>

#include <Crioulo/renderer.h>
#include <Crioulo/internal/debug_gl.h>


using namespace Crioulo;

Renderer::Renderer(IContext& context) :
    m_contextManager(GlobalContextManager::getInstance()),
    m_context(context),
    m_camera(Camera{}),
    m_surfaceRatio(1.0f)
{
    if (!isInitialized)
        throw std::runtime_error("OpenGL functions have not been initialized");

    m_contextManager.makeCurrent(m_context);

    // Setting up Debugging
    int flags = 0; 
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback((GLDEBUGPROC)logOpenGL, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glEnable(GL_DEPTH_TEST);
    
    // Creating Uniform Buffer Objects
    glGenBuffers(1, &m_uboTransformMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uboTransformMatrices);
    constexpr GLuint transformMatricesSize = sizeof(glm::mat4) * 2;
    glBufferData(GL_UNIFORM_BUFFER, transformMatricesSize, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uboTransformMatrices);

    glGenBuffers(1, &m_uboPointLights);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uboPointLights);
    constexpr GLuint pointLightsBlockSize = 4 * sizeof(PointLight);
    glBufferData(GL_UNIFORM_BUFFER, pointLightsBlockSize, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uboPointLights);

}

void Renderer::drawScene()
{
    ZoneScoped;
    m_contextManager.makeCurrent(m_context);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_UNIFORM_BUFFER, m_uboTransformMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(m_camera.getViewTransformationMatrix()));
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(m_camera.getProjectionTransformationMatrix(m_surfaceRatio)));

    glBindBuffer(GL_UNIFORM_BUFFER, m_uboPointLights);
    unsigned int i = 0;
    for (const auto& pointLight : m_pointLights) {
        const GLintptr offset = i + sizeof(PointLight);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(PointLight), (void*)&pointLight);
        i++;
    } 
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    for (MeshInstance* instance : m_instances)
    {
        instance->draw();
    }
}

std::shared_ptr<Texture> Renderer::loadTexture(const TextureData& data)
{
    m_contextManager.makeCurrent(m_context);
    Texture* texture = new Texture(data);

    auto deleter = [this](Texture* texture) {
        this->m_contextManager.makeCurrent(m_context);
        delete texture;
    };

    std::shared_ptr<Texture> ptr(texture, deleter);
    return ptr;
}

std::shared_ptr<Shader> Renderer::loadShader(const char* vertexCode, const char* fragmentCode)
{
    m_contextManager.makeCurrent(m_context);
    Shader* shader = new Shader(vertexCode, fragmentCode);

    auto deleter = [this](Shader* shader) {
        this->m_contextManager.makeCurrent(m_context);
        delete shader;
    };

    std::shared_ptr<Shader> ptr(shader, deleter);

    shader->setUniformBlockBinding("TransformMatrices", 0);
    shader->setUniformBlockBinding("PointLights", 1);

    return ptr;
}

MeshInstance* Renderer::createMeshInstance(std::shared_ptr<Mesh> mesh, const Material& material)
{
    MeshInstance* instance = new MeshInstance(mesh, material);
    m_instances.insert(instance);
    return instance;
}

void Renderer::destroyMeshInstance(MeshInstance* instance) {
    m_instances.erase(instance);
    delete instance;
}

PointLight* Renderer::createPointLight()
{
    PointLight* light = new PointLight();
    m_pointLights.insert(light);
    return light;
}

void Renderer::destroyPointLight(PointLight* light) {
    m_pointLights.erase(light);
    delete light;
}

void Renderer::bindOpenGLFunctions(void* (* getProcAddress)(const char*))
{
    if (isInitialized) return;
    if (!gladLoadGLLoader((GLADloadproc)getProcAddress))
    {
        throw std::runtime_error("Failed to initialize OpenGL functions");
    }
    isInitialized = true;
}







