#ifndef CRIOULO_H 
#define CRIOULO_H

#include <vector>
#include <unordered_set>
#include <memory>
#include "global_context_manager.h"
#include "camera.h"
#include "material.h"
#include "mesh_instance.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "light.h"

#define DEPTH_TEST_FUNCTION_LIST \
    X(Always, GL_ALWAYS) \
    X(Never, GL_NEVER) \
    X(Less,  GL_LESS) \
    X(LessOrEqual, GL_LEQUAL) \
    X(Equal, GL_EQUAL) \
    X(GreaterOrEqual, GL_GEQUAL) \
    X(Greater, GL_GREATER) \
    X(NotEqual, GL_NOTEQUAL)

#define CULLING_FACE_LIST \
    X(BackFace, GL_BACK) \
    X(FrontFace, GL_FRONT)

#define CULLING_FRONT_LIST \
    X(Clockwise, GL_CW) \
    X(CounterClockwise, GL_CCW)


namespace Crioulo 
{
    enum class DepthTestFunction {
#define X(name, gl_val) name,
        DEPTH_TEST_FUNCTION_LIST
#undef X
    };


    enum class CullingFace {
#define X(name, gl_val) name,
        CULLING_FACE_LIST
#undef X
    };


    enum class CullingFront {
#define X(name, gl_val) name,
        CULLING_FRONT_LIST
#undef X
    };


    struct RendererSettings {
        bool depthTest = true;
        DepthTestFunction depthTestFunction = DepthTestFunction::Less;
        bool faceCulling = true;
        CullingFace cullingFace = CullingFace::BackFace;
        CullingFront cullingFront = CullingFront::CounterClockwise;
    };


    class Renderer
    {
        public:

            Renderer(IContext& contexts, const RendererSettings& settings);
            Renderer(const Renderer& other) = delete;
            Renderer(Renderer&& other) = delete;
            ~Renderer() = default;

            Renderer* operator=(const Renderer& other) = delete;
            Renderer* operator=(Renderer&& other) = delete;

            void drawScene();

            template <typename T>
            std::shared_ptr<Mesh> loadMesh(const MeshData<T>& data)
            {
                m_contextManager.makeCurrent(m_context);
                Mesh* mesh = new Mesh(data);

                auto deleter = [this](Mesh* mesh) {
                    this->m_contextManager.makeCurrent(m_context);
                    delete mesh;
                    };

                std::shared_ptr<Mesh> ptr(mesh, deleter);
                return ptr;
            }
            std::shared_ptr<Texture> loadTexture(const TextureData& data);
            std::shared_ptr<Shader> loadShader(const char* vertexCode, const char* fragmentCode);

            MeshInstance* createMeshInstance(std::shared_ptr<Mesh> mesh, const Material& material);
            PointLight* createPointLight();

            void destroyMeshInstance(MeshInstance* instance);
            void destroyPointLight(PointLight* light);
            
            inline void adjustViewport(int x, int y, int width, int height) {
                m_contextManager.makeCurrent(m_context);
                glViewport(0, 0, width, height);
            }

            inline void setCamera(const Camera& camera) {
                m_camera = camera;
            }

            inline void setSurfaceRatio(float surfaceRatio){
                m_surfaceRatio = surfaceRatio;
            }
        
            static void bindOpenGLFunctions(void* (* getProcAddress)(const char*));

        private:

            inline static bool isInitialized = false;

            GlobalContextManager& m_contextManager;
            IContext& m_context;

            float m_surfaceRatio;
            
            Camera m_camera;
            std::unordered_set<MeshInstance*> m_instances;
            std::unordered_set<PointLight*> m_pointLights;

            unsigned int m_uboTransformMatrices;
            unsigned int m_uboPointLights;
    };
}
#endif