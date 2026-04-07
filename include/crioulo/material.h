#ifndef CRIOULO_MATERIAL_H
#define CRIOULO_MATERIAL_H

#include <tracy/Tracy.hpp>
#include <vector>
#include <memory>
#include <string>
#include <utility>

#include "texture.h"
#include "shader.h"

namespace Crioulo
{
    struct TextureSlot
    {
        std::shared_ptr<Texture> texture;
        std::string slot;
    };

    struct Material
    {
        friend class Renderer;
        friend class MeshInstance;
        
        Material(const std::shared_ptr<Shader>& shader, const std::vector<TextureSlot>& textureSlots) :
            m_shader(shader),
            m_textures(textureSlots)
        {
            if (!shader) {
                throw std::runtime_error("Shader must not be nullptr.");
            }

            for (const auto& slot : textureSlots) {
                if (!slot.texture) {
                    throw std::runtime_error("Texture must not be nullptr.");
                }
            }
        }
        Material(const Material& other) = default;


        void addTextureSlot(const TextureSlot& slot) {
            if (!slot.texture) {
                throw std::runtime_error("Texture must not be nullptr.");
            }
            m_textures.push_back(slot);
        }

    private:

        std::shared_ptr<Shader> m_shader;
        std::vector<TextureSlot> m_textures;

        inline unsigned int getUniformLocation(const std::string& name) {
            return m_shader->getUniformLocation(name);
        }

        inline void use() {
            ZoneScoped;
            m_shader->use();

            unsigned int i = 0;
            for (const TextureSlot& textureSlot : m_textures)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glUniform1i(m_shader->getUniformLocation(textureSlot.slot), i);
                textureSlot.texture->bindTexture();
                i++;
            }
        }
    };
}
#endif