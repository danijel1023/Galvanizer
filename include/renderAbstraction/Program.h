#pragma once
#include <string>
#include <vector>

namespace Galvanizer::RenderAbstraction
{
enum class ShaderType { Vertex, Fragment };


class Program
{
public:
    ~Program();

    bool CreateShader(std::string_view src, ShaderType type);
    bool CompileProgram();


    bool SetUniformI(const std::string&, int a) const;
    bool SetUniformF(const std::string&, float a) const;
    bool SetUniformD(const std::string&, double a) const;
    bool SetUniformI(const std::string&, int a, int b) const;
    bool SetUniformF(const std::string&, float a, float b) const;
    bool SetUniformD(const std::string&, double a, double b) const;
    bool SetUniformI(const std::string&, int a, int b, int c) const;
    bool SetUniformF(const std::string&, float a, float b, float c) const;
    bool SetUniformD(const std::string&, double a, double b, double c) const;
    bool SetUniformI(const std::string&, int a, int b, int c, int d) const;
    bool SetUniformF(const std::string&, float a, float b, float c, float d) const;
    bool SetUniformD(const std::string&, double a, double b, double c, double d) const;

    void Bind() const;
    static void Unbind();

private:
    std::vector<unsigned int> m_shaders;
    unsigned int m_program = 0;
};
}
