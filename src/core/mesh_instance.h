#ifndef CRIOULO_MESH_INSTANCE_H
#define CRIOULO_MESH_INSTANCE_H

#include <vector>
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

            glm::mat4 m_transform;
            std::shared_ptr<Material> m_material;
            std::shared_ptr<Mesh> m_mesh;

            /*inline void draw()
            {
                m_mesh->bind();

                m_material->m_shader->use();

                glUniformMatrix4fv(glGetUniformLocation(m_material->m_shader->m_id, "model"), 1, GL_FALSE, glm::value_ptr(m_transform));

                unsigned int i = 0;
                for(const TextureSlot& textureSlot : m_material->m_textures)
                {
                    glActiveTexture(GL_TEXTURE0 + i);
                    glUniform1i(glGetUniformLocation(m_material->m_shader->m_id, textureSlot.slot.c_str()), i);
                    glBindTexture(GL_TEXTURE_2D, textureSlot.texture->m_id);
                    i++;
                }

                glDrawArrays(GL_TRIANGLES, 0, instance->m_mesh->m_vertexCount);
            }*/
    };
};
#endif