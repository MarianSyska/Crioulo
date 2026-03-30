#ifndef CRIOULO_SHADER_H
#define CRIOULO_SHADER_H


#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Crioulo
{

    class Shader
    {
        friend class Renderer;
        friend class Material;

        private:

            unsigned int m_id;

            inline ~Shader()
            {
                glDeleteProgram(m_id);
            }

            inline Shader(const char* vertexCode, const char* fragmentCode)
            {
                unsigned int vertexId = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vertexId, 1, &vertexCode, NULL);
                glCompileShader(vertexId);

                checkCompileErrors(vertexId, "VERTEX");

                unsigned int fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragmentId, 1, &fragmentCode, NULL);
                glCompileShader(fragmentId);

                checkCompileErrors(fragmentId, "FRAGMENT");

                m_id = glCreateProgram();
                glAttachShader(m_id, vertexId);
                glAttachShader(m_id, fragmentId);
                glLinkProgram(m_id);

                checkCompileErrors(m_id, "PROGRAM");

                glDeleteShader(vertexId);
                glDeleteShader(fragmentId);
            }

            inline void use() 
            {
                glUseProgram(m_id); 
            }

            inline unsigned int getUniformLocation(const std::string& name) {
                return glGetUniformLocation(m_id, name.c_str());
            }

            inline bool setUniformBlockBinding(const std::string& name, unsigned int binding) {
                unsigned int uniformIndex = glGetUniformBlockIndex(m_id, name.c_str());
                if (uniformIndex == GL_INVALID_INDEX) {
                    return false;
                }
                glUniformBlockBinding(m_id, uniformIndex, binding);
                return true;
            }

            inline void checkCompileErrors(unsigned int shader, std::string type)
            {
                int success;
                char infoLog[1024];
                if (type != "PROGRAM")
                {
                    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                    if (!success)
                    {
                        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                    }
                }
                else
                {
                    glGetProgramiv(shader, GL_LINK_STATUS, &success);
                    if (!success)
                    {
                        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                    }
                }
            }
    };
};
#endif