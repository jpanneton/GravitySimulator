#include "Shader.h"
#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader() {

}

void Shader::loadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    // Create the shaders
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the shaders
    std::string vertexShaderCode = readShader(vertexShaderPath);
    std::string fragmentShaderCode = readShader(fragmentShaderPath);
    const GLchar* vertexShaderCodePtr = vertexShaderCode.c_str();
    const GLchar* fragmentShaderCodePtr = fragmentShaderCode.c_str();

    // Compile the shaders
    glShaderSource(vertexShaderID, 1, &vertexShaderCodePtr, nullptr);
    glCompileShader(vertexShaderID);
    validateShader(vertexShaderID, vertexShaderPath);

    glShaderSource(fragmentShaderID, 1, &fragmentShaderCodePtr, nullptr);
    glCompileShader(fragmentShaderID);
    validateShader(fragmentShaderID, fragmentShaderPath);

    // Shader Program
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertexShaderID);
    glAttachShader(m_programID, fragmentShaderID);
    glLinkProgram(m_programID);
    validateProgram();

    // Delete the shaders as they're linked into our program now and no longer necessery
    glDetachShader(m_programID, vertexShaderID);
    glDetachShader(m_programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}

Shader::~Shader()
{
    glDeleteProgram(m_programID);
}

GLuint Shader::id() const
{
    return m_programID;
}

void Shader::bind() const
{
    glUseProgram(m_programID);
}

void Shader::unbind() const
{
    glUseProgram(0);
}

std::string Shader::readShader(const std::string& filePath) const
{
    std::ifstream fileStream(filePath);

    if (fileStream)
    {
        std::ostringstream buffer;
        if (buffer << fileStream.rdbuf())
        {
            return buffer.str();
        }
        std::cerr << "Unable to read file " << filePath << std::endl;
    }
    else
    {
        std::cerr << "Unable to open file " << filePath << std::endl;
    }

    return std::string{};
}

void Shader::validateShader(GLuint shaderID, const std::string& filePath) const
{
    GLint success;
    GLchar infoLog[512];

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        std::cerr << "Vertex shader " << shaderID << " (" << filePath << ") compilation failed: " << infoLog << std::endl;
    };
}

void Shader::validateProgram() const
{
    GLint success;
    GLchar infoLog[512];

    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_programID, 512, nullptr, infoLog);
        std::cerr << "Program " << m_programID << " linking failed: " << infoLog << std::endl;
    }

    glValidateProgram(m_programID);
    glGetProgramiv(m_programID, GL_VALIDATE_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_programID, 512, nullptr, infoLog);
        std::cerr << "Program " << m_programID << " validation failed: " << infoLog << std::endl;
    }
}