#pragma once

#include <GL/glew.h>
#include <string>

class Shader
{
public:
    Shader();
    ~Shader();

    GLuint id() const;
    void bind() const;
    void unbind() const;
    void loadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

private:	
    std::string readShader(const std::string& filePath) const;
    void validateShader(GLuint shaderID, const std::string& filePath) const;
    void validateProgram() const;
    GLuint m_programID;
};