#ifndef CRIOULO_TEXTURE_H
#define CRIOULO_TEXTURE_H


#include <glad/glad.h>
#include <string>

namespace Crioulo
{
    class Renderer;

    struct Texture2DData
    {
        int height;
        int width;
        int nrComponents;
        unsigned char* data;
    };

    struct CubeMapTextureData {
        int height[6];
        int width[6];
        int nrComponents[6];
        unsigned char* data[6];
    };

    class Texture
    {
        friend class Renderer;
        friend class Material;

        private:
            unsigned int m_id;
            int m_type;

            inline Texture(const Texture2DData& data) :
                m_type(GL_TEXTURE_2D)
            {
                GLenum format = GL_RED;
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

            inline Texture(const CubeMapTextureData& data) :
                m_type(GL_TEXTURE_CUBE_MAP)
            {
                glGenTextures(1, &m_id);
                glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

                for (unsigned int i = 0; i < 6; i++) {
                    GLenum format = GL_RED;
                    if (data.nrComponents[i] == 1)
                        format = GL_RED;
                    else if (data.nrComponents[i] == 3)
                        format = GL_RGB;
                    else if (data.nrComponents[i] == 4)
                        format = GL_RGBA;

                    glTexImage2D(
                        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                        0,
                        format,
                        data.width[i],
                        data.height[i],
                        0,
                        format,
                        GL_UNSIGNED_BYTE,
                        data.data[i]
                    );
                }
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            }

            ~Texture() {
                glDeleteTextures(1, &m_id);
            }

            inline void bindTexture() {
                glBindTexture(m_type, m_id);
            }
    };
};
#endif