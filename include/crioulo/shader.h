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
    class Material;

    class Shader
    {
        friend class Renderer;
        friend class Material;

        public:

            ~Shader()
            {
                glDeleteProgram(m_id);
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

            void setInt(const std::string &name, GLint* value) const
            { 
                glUniform1iv(glGetUniformLocation(m_id, name.c_str()), 1, value); 
            }

            void setUInt(const std::string &name, GLuint* value)
            {
                glUniform1uiv(glGetUniformLocation(m_id, name.c_str()), 1, value);
            }

            void setFloat(const std::string &name, GLfloat* value) const
            { 
                glUniform1fv(glGetUniformLocation(m_id, name.c_str()), 1, value); 
            }

            void setFloat2(const std::string &name, GLfloat* value) const
            { 
                glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, value); 
            }

            void setFloat3(const std::string &name, GLfloat* value) const
            { 
                glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, value); 
            }

            void setFloat4(const std::string &name, GLfloat* value) const
            { 
                glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, value); 
            }

            void setMat2(const std::string &name, GLfloat* value)
            {
                glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, value);
            }

            void setMat3(const std::string &name, GLfloat* value)
            {
                glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, value);
            }

            void setMat4(const std::string &name, GLfloat* value)
            {
                glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, value);
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