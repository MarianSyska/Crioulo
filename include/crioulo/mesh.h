#ifndef CRIOULO_MESH_H
#define CRIOULO_MESH_H

#include <tracy/Tracy.hpp>
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>


namespace Crioulo
{
    struct VertexAttributeDetails {
        GLuint index;
        GLint count;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        const GLvoid* offset;
    };

    template <typename T>
    concept VertexAttributeObject = requires {
        { T::attributes };

        requires std::same_as<
            std::remove_cvref_t<decltype(*std::begin(T::attributes))>,
                VertexAttributeDetails
        >;
    };

    template <VertexAttributeObject V>
    struct MeshData
    {
        std::vector<V> vertices;
        std::vector<unsigned int> indices;
    };

    class Mesh {
        
        friend class Renderer;
        friend class MeshInstance;

        private:
            template <VertexAttributeObject V>
            inline Mesh(const MeshData<V>& data) :
                m_vertexCount(static_cast<unsigned int>(data.indices.size()))
            {
                glGenVertexArrays(1, &m_vao);
                glGenBuffers(1, &m_vbo);
                glGenBuffers(1, &m_ebo);

                glBindVertexArray(m_vao);

                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(V), &data.vertices[0], GL_STATIC_DRAW);  

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), &data.indices[0], GL_STATIC_DRAW);

                for (const VertexAttributeDetails& details : V::attributes) {
                    glEnableVertexAttribArray(details.index);
                    glVertexAttribPointer(details.index, details.count, details.type, details.normalized, details.stride, details.offset);
                }

                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }

            inline ~Mesh() {
                glDeleteVertexArrays(1, &m_vao);
                glDeleteBuffers(1, &m_vbo);
                glDeleteBuffers(1, &m_ebo);
            }

            inline void draw() const {
                ZoneScoped;
                glBindVertexArray(m_vao);
                glDrawElements(GL_TRIANGLES, m_vertexCount, GL_UNSIGNED_INT, (void*) 0);
                glBindVertexArray(NULL);
            }

            unsigned int m_vbo, m_ebo, m_vao;
            const unsigned int m_vertexCount;
    };
};
#endif