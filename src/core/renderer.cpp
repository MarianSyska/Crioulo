#include <tracy/Tracy.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <string>
#include <utility>
#include <ranges>
#include <array>
#include <shaders/internal/skybox_frag.hpp>
#include <shaders/internal/skybox_vert.hpp>

#include <Crioulo/renderer.h>
#include <Crioulo/internal/debug_gl.h>
#include <Crioulo/internal/primitive_shapes.h>

using namespace Crioulo;

namespace {
    struct SimpleVertex {
        glm::vec3 position;
        static const std::array<VertexAttributeDetails, 1> attributes;
    };

    const std::array<VertexAttributeDetails, 1> SimpleVertex::attributes = {
           { 0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*) offsetof(SimpleVertex, position)}
    };

    const std::array<SimpleVertex, 8> cubeVertices{{
            // Front face
            { { -1.0f,  1.0f,  1.0f } }, // 0: Top-left
            { { -1.0f, -1.0f,  1.0f } }, // 1: Bottom-left
            { {  1.0f, -1.0f,  1.0f } }, // 2: Bottom-right
            { {  1.0f,  1.0f,  1.0f } }, // 3: Top-right

            // Back face
            { { -1.0f,  1.0f, -1.0f } }, // 4: Top-left
            { { -1.0f, -1.0f, -1.0f } }, // 5: Bottom-left
            { {  1.0f, -1.0f, -1.0f } }, // 6: Bottom-right
            { {  1.0f,  1.0f, -1.0f } }  // 7: Top-right
    }};

    constexpr std::array<unsigned int, 36> cubeIndices{ {
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
    } };

    constexpr auto DEPTH_TEST_FUNCTION_MAPPING = std::to_array<int>({
#define X(name, gl_val) gl_val,
        DEPTH_TEST_FUNCTION_LIST
#undef X
    });

    constexpr auto CULLING_FACE_MAPPING = std::to_array<int>({
#define X(name, gl_val) gl_val,
        CULLING_FACE_LIST
#undef X
    });

    constexpr auto CULLING_FRONT_MAPPING = std::to_array<int>({
#define X(name, gl_val) gl_val,
        CULLING_FRONT_LIST
#undef X
    });

    int toGlEnum(DepthTestFunction in) {
        return DEPTH_TEST_FUNCTION_MAPPING[static_cast<int>(in)];
    }

    int toGlEnum(CullingFace in) {
        return CULLING_FACE_MAPPING[static_cast<int>(in)];
    }

    int toGlEnum(CullingFront in) {
        return CULLING_FRONT_MAPPING[static_cast<int>(in)];
    }
};

Renderer::Renderer(IContext& context, const RendererSettings& settings) :
    m_contextManager(GlobalContextManager::getInstance()),
    m_context(context),
    m_camera(Camera{}),
    m_surfaceRatio(1.0f),
    m_skyBox(nullptr)
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

    // Set up
    if (settings.depthTest) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(toGlEnum(settings.depthTestFunction));
    }
    
    if (settings.faceCulling) {
        glEnable(GL_CULL_FACE);
        glCullFace(toGlEnum(settings.cullingFace));
        glFrontFace(toGlEnum(settings.cullingFront));
    }    

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

Crioulo::Renderer::~Renderer(){
    for (auto pointLight : m_pointLights) {
        delete pointLight;
    }

    for (auto instance : m_instances) {
        delete instance;
    }

    if (!m_skyBox) {
        delete m_skyBox;
    }
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

    // Draw SkyBox
    if (m_skyBox) {
        glDepthMask(GL_FALSE);
        m_skyBox->draw();
        glDepthMask(GL_TRUE);
    }

    for (MeshInstance* instance : m_instances)
    {
        instance->draw();
    }
}

std::shared_ptr<Texture> Renderer::loadTexture(const Texture2DData& data)
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

std::shared_ptr<Texture> Crioulo::Renderer::loadTexture(const CubeMapTextureData& data)
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

void Crioulo::Renderer::setSkyBox(std::shared_ptr<Texture> texture) {
    if (texture->m_type != GL_TEXTURE_CUBE_MAP) {
        throw std::runtime_error("Texture of SkyBox must be a cube map.");
    }

    std::shared_ptr<Shader> shader = loadShader(Shaders::Internal::skybox_vert.data(), Shaders::Internal::skybox_frag.data());

    using namespace PrimitiveShapes;
    MeshData<SimpleVertex> data{ 
        std::vector<SimpleVertex>(UNIT_CUBE.vertices.begin(), UNIT_CUBE.vertices.end()),
        std::vector<unsigned int>(UNIT_CUBE.indices.begin(), UNIT_CUBE.indices.end())
    };

    std::shared_ptr<Mesh> mesh = loadMesh(data);
    Material material(shader, {{texture, "skyBox"}});

    if (!m_skyBox) {
        delete m_skyBox;
    }
    m_skyBox = new MeshInstance(mesh, material);
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
