#ifndef CRIOULO_MATERIAL_H
#define CRIOULO_MATERIAL_H

#include <vector>
#include <memory>
#include <string>
#include <utility>

#include "texture.h"
#include "shader.h"

namespace Crioulo
{
    class Renderer;
    class MeshInstance;

    enum UniformType{
        FLOAT,
        FLOAT2,
        FLOAT3,
        FLOAT4,
        INT,
        INT2,
        INT3,
        INT4,
        UINT,
        UINT2,
        UINT3,
        UINT4,
        MATRIX_FLOAT2,
        MATRIX_FLOAT3,
        MATRIX_FLOAT4,
        MATRIX_FLOAT2_3,
        MATRIX_FLOAT3_2,
        MATRIX_FLOAT2_4,
        MATRIX_FLOAT4_2,
        MATRIX_FLOAT3_4,
        MATRIX_FLOAT4_3
    };

    struct TextureSlot
    {
        std::shared_ptr<Texture> texture;
        std::string slot;
    };

    struct UniformSlot
    {
        const char* name;
        UniformType type;
        void* data;
    };

    class Material
    {
        friend class Renderer;
        friend class MeshInstance;

        public:
        
            Material(const std::shared_ptr<Shader>& shader, const std::vector<TextureSlot>& textures)
            {
                m_shader = shader;
                m_textures = textures;
            }

        private:

            inline void apply()
            {
                m_shader->use();
                unsigned int i = 0;
                for(const TextureSlot& textureSlot : m_textures)
                {
                    glActiveTexture(GL_TEXTURE0 + i);
                    glUniform1i(glGetUniformLocation(m_shader->m_id, textureSlot.slot.c_str()), i);
                    glBindTexture(GL_TEXTURE_2D, textureSlot.texture->m_id);
                    i++;
                }
            }

            inline void setUniforms(const std::vector<UniformSlot>& uniformSlots)
            {
                for(const UniformSlot& uniformSlot : uniformSlots)
                {
                    switch (uniformSlot.type)
                    {
                        case FLOAT:
                            m_shader->setFloat(uniformSlot.name, static_cast<float*>(uniformSlot.data));
                            break;
                        case FLOAT2:
                            m_shader->setFloat2(uniformSlot.name, static_cast<float*>(uniformSlot.data));
                            break;
                        case FLOAT3:
                            m_shader->setFloat3(uniformSlot.name, static_cast<float*>(uniformSlot.data));
                            break;
                        case FLOAT4:
                            m_shader->setFloat4(uniformSlot.name, static_cast<float*>(uniformSlot.data));
                            break;
                        case INT:
                            m_shader->setInt(uniformSlot.name, static_cast<int*>(uniformSlot.data));
                            break;
                        case INT2:
                            break;
                        case INT3:
                            break;
                        case INT4:
                            break;
                        case UINT:
                            m_shader->setUInt(uniformSlot.name, static_cast<unsigned int*>(uniformSlot.data));
                            break;
                        case UINT2:
                            break;
                        case UINT3:
                            break;
                        case UINT4:
                            break;
                        case MATRIX_FLOAT2:
                            m_shader->setMat2(uniformSlot.name, static_cast<float*>(uniformSlot.data));
                            break;
                        case MATRIX_FLOAT3:
                            m_shader->setMat3(uniformSlot.name, static_cast<float*>(uniformSlot.data));
                            break;
                        case MATRIX_FLOAT4:
                            m_shader->setMat4(uniformSlot.name, static_cast<float*>(uniformSlot.data));
                            break;
                        case MATRIX_FLOAT2_3:
                            break;
                        case MATRIX_FLOAT3_2:
                            break;
                        case MATRIX_FLOAT2_4:
                            break;
                        case MATRIX_FLOAT4_2:
                            break;
                        case MATRIX_FLOAT3_4:
                            break;
                        case MATRIX_FLOAT4_3:
                            break;
                    }
                }
            }

            std::shared_ptr<Shader> m_shader;
            std::vector<TextureSlot> m_textures;
    };
}
#endif