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

    struct TextureSlot
    {
        std::shared_ptr<Texture> texture;
        std::string slot;
    };

    class Material
    {
        friend class Renderer;

        public:
        
            Material(const std::shared_ptr<Shader>& shader, const std::vector<TextureSlot>& textures)
            {
                m_shader = shader;
                m_textures = textures;
            }

        private:

            std::shared_ptr<Shader> m_shader;
            std::vector<TextureSlot> m_textures;
    };
}
#endif