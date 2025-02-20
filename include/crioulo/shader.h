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
    class Renderer;

    class Shader
    {
        friend class Renderer;

        public:

            ~Shader()
            {
                glDeleteProgram(m_id);
            }

            void setBool(const std::string &name, bool value) const
            {         
                glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value); 
            }
            
            void setInt(const std::string &name, int value) const
            { 
                glUniform1i(glGetUniformLocation(m_id, name.c_str()), value); 
            }

            void setFloat(const std::string &name, float value) const
            { 
                glUniform1f(glGetUniformLocation(m_id, name.c_str()), value); 
            }

            void setMat4(const std::string &name, glm::mat4 value)
            {
                glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
            }

        private:

            unsigned int m_id;

            Shader(const char* vertexCode, const char* fragmentCode)
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

            void use() 
            { 
                glUseProgram(m_id); 
            }

            void checkCompileErrors(unsigned int shader, std::string type)
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