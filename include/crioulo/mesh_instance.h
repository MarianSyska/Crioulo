#ifndef CRIOULO_MESH_INSTANCE_H
#define CRIOULO_MESH_INSTANCE_H

#include <tracy/Tracy.hpp>
#include <memory>
#include <glm/mat4x4.hpp>

#include "mesh.h"
#include "material.h"

namespace Crioulo
{
    class MeshInstance
    {
        friend class Renderer;

    public:

        inline void setTransform(const glm::mat4& transform) {
            m_transform = transform;
        };

    private:

        MeshInstance(std::shared_ptr<Mesh> mesh, const Material& material) : 
            m_mesh(mesh),
            m_material(material),
            m_transform(glm::mat4(0.0f))
        {
            if (!mesh) {
                throw std::runtime_error("Mesh must not be nullptr.");
            }
        }

        ~MeshInstance() = default;

        std::shared_ptr<Mesh> m_mesh;
        Material m_material;
        glm::mat4 m_transform;

        inline void draw() {
            ZoneScoped;
            m_material.use();
            constexpr GLsizei uniformCount = 1;
            constexpr GLboolean shouldTranspose = GL_FALSE;
            glUniformMatrix4fv(m_material.getUniformLocation("model"), uniformCount, shouldTranspose, glm::value_ptr(m_transform));
            m_mesh->draw();
        }
    };
};
#endif