#ifndef CRIOULO_MESH_INSTANCE_H
#define CRIOULO_MESH_INSTANCE_H

#include <vector>
#include <array>
#include <memory>
#include <glm/mat4x4.hpp>

#include "mesh.h"
#include "material.h"

namespace Crioulo
{
    class Renderer;

    class MeshInstance
    {
        friend class Renderer;

        public:
            
            MeshInstance(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material)
            {
                m_mesh = mesh;
                m_material = material;
            }

            inline void setTransform(glm::mat4 transform)
            {
                m_transform = transform;
            }

        private:

            inline void draw(std::vector<UnifromSlot>& uniformSlots)
            {
                uniformSlots.push_back({"model", UniformType.MATRIX_FLOAT4, glm::value_ptr(m_transform)});

                m_material->apply();

                m_material->setUniforms(uniformSlots);
            
                m_mesh->draw();
            }

            glm::mat4 m_transform;
            std::shared_ptr<Material> m_material;
            std::shared_ptr<Mesh> m_mesh;

    };
};
#endif