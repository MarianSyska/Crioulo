#ifndef CRIOULO_TEXTURE_H
#define CRIOULO_TEXTURE_H


#include <glad/glad.h>
#include <string>

namespace Crioulo
{
    class Renderer;
    class Material;

    struct TextureData
    {
        int height;
        int width;
        int nrComponents;
        unsigned char* data;
    };

    class Texture
    {
        friend class Renderer;
        friend class Material;

        public:

            ~Texture()
            {
                glDeleteTextures(1, &m_id);
            }

        private:
            unsigned int m_id;

            Texture(const TextureData& data) 
            {
                GLenum format;
                if (data.nrComponents == 1)
                    format = GL_RED;
                else if (data.nrComponents == 3)
                    format = GL_RGB;
                else if (data.nrComponents == 4)
                    format = GL_RGBA;

                glGenTextures(1, &m_id);
                glBindTexture(GL_TEXTURE_2D, m_id);
                glTexImage2D(GL_TEXTURE_2D, 0, format, data.width, data.height, 0, format, GL_UNSIGNED_BYTE, data.data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glBindTexture(GL_TEXTURE_2D, 0);
            }
    };
};
#endif