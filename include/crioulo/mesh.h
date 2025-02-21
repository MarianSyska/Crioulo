#ifndef CRIOULO_MESH_H
#define CRIOULO_MESH_H

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>


namespace Crioulo
{
    class Renderer;
    class MeshInstance;

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };
    
    struct MeshData
    {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
    };

    class Mesh {
        
        friend class Renderer;
        friend class MeshInstance;
        
        public:

            ~Mesh()
            {
                glDeleteVertexArrays(1, &m_vao);
                glDeleteBuffers(1, &m_vbo);
                glDeleteBuffers(1, &m_ebo);
            }

        private:
            unsigned int m_vbo, m_ebo, m_vao;
            unsigned int m_indeciesCount;

            Mesh(const MeshData& data)
            {
                m_indeciesCount = static_cast<unsigned int>(data.indices.size());

                glGenVertexArrays(1, &m_vao);
                glGenBuffers(1, &m_vbo);
                glGenBuffers(1, &m_ebo);

                glBindVertexArray(m_vao);

                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(Vertex), &data.vertices[0], GL_STATIC_DRAW);  

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), &data.indices[0], GL_STATIC_DRAW);

                glEnableVertexAttribArray(0);	
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

                glEnableVertexAttribArray(1);	
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

                glEnableVertexAttribArray(2);	
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
            }

            inline void draw() const
            {
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
                glBindVertexArray(m_vao);
                glDrawArrays(GL_TRIANGLES, 0, m_indeciesCount);
            }
    };
};
#endif