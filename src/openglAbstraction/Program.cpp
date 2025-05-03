#include <cstdint>
#include <iostream>

#include "openglAbstraction/Program.h"

#include "glad/glad.h"
#include "GLErr.inl"


using namespace Galvanizer::RenderAbstraction;

Program::~Program()
{
    GLCall(glDeleteProgram(m_program));
    for (const auto& id: m_shaders)
        GLCall(glDeleteShader(id));
}


bool Program::CreateShader(std::string_view src, ShaderType type)
{
    GLenum glType;
    switch (type)
    {
    case ShaderType::Fragment:
        glType = GL_FRAGMENT_SHADER;
        break;

    case ShaderType::Vertex:
        glType = GL_VERTEX_SHADER;
        break;

    default:
        glType = 0;
        break;
    }

    unsigned int id = glCreateShader(glType);

    const char* data = src.data();
    glShaderSource(id, 1, &data, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        std::string msg;
        msg.resize(length);

        glGetShaderInfoLog(id, length, &length, msg.data());
        std::cout << "[Error::Compile_Shader - msg length: " << length << "] " << msg << std::endl;

        glDeleteShader(id);
        return false;
    }

    m_shaders.push_back(id);
    return id;
}

bool Program::CompileProgram()
{
    m_program = glCreateProgram();

    // Linking stage
    for (const auto& id: m_shaders)
        glAttachShader(m_program, id);
    glLinkProgram(m_program);


    int result = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);

        std::string msg;
        msg.resize(length);

        glGetProgramInfoLog(m_program, length, &length, msg.data());
        std::cout << "[Error::Link_Program - msg length: " << length << "] " << msg << std::endl;

        glDeleteProgram(m_program);
        return false;
    }

    glValidateProgram(m_program);

    // Delete unused shaders
    for (const auto& id: m_shaders)
        glDeleteShader(id);

    return m_program;
}

void Program::Bind() const
{
    GLCall(glUseProgram(m_program));
}

void Program::Unbind()
{
    GLCall(glUseProgram(0));
}

bool Program::SetUniformI(const std::string& uniform, int a) const
{
    int uniformLoc = glGetUniformLocation(m_program, uniform.c_str());
    if (uniformLoc == -1)
        return false;

    Bind();
    GLCall(glUniform1i(uniformLoc, a));
    return true;
}

bool Program::SetUniformF(const std::string& uniform, float a) const { return false; }
bool Program::SetUniformD(const std::string& uniform, double a) const { return false; }

bool Program::SetUniformI(const std::string& uniform, int a, int b) const
{
    int uniformLoc = glGetUniformLocation(m_program, uniform.c_str());
    if (uniformLoc == -1)
        return false;

    Bind();
    GLCall(glUniform2i(uniformLoc, a, b));
    return true;
}

bool Program::SetUniformF(const std::string& uniform, float a, float b) const
{
    int uniformLoc = glGetUniformLocation(m_program, uniform.c_str());
    if (uniformLoc == -1)
        return false;

    Bind();
    GLCall(glUniform2f(uniformLoc, a, b));
    return true;
}

bool Program::SetUniformD(const std::string& uniform, double a, double b) const { return false; }
bool Program::SetUniformI(const std::string& uniform, int a, int b, int c) const { return false; }
bool Program::SetUniformF(const std::string& uniform, float a, float b, float c) const { return false; }
bool Program::SetUniformD(const std::string& uniform, double a, double b, double c) const { return false; }
bool Program::SetUniformI(const std::string& uniform, int a, int b, int c, int d) const { return false; }
bool Program::SetUniformF(const std::string& uniform, float a, float b, float c, float d) const { return false; }
bool Program::SetUniformD(const std::string& uniform, double a, double b, double c, double d) const { return false; }
