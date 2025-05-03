#include <iostream>
#include <unordered_map>

#include "openglAbstraction/VertexSetup.h"
#include "openglAbstraction/Mesh.h"

#include "GLErr.inl"
#include "glad/glad.h"


using namespace Galvanizer::RenderAbstraction;

VertexSetup::VertexSetup(Mesh* mesh)
    : m_mesh(mesh) {}

void VertexSetup::Init()
{
    GLCall(glGenVertexArrays(1, &m_VA));
}

void VertexSetup::Exit()
{
    GLCall(glDeleteVertexArrays(1, &m_VA));
}



struct VAItem
{
    GLenum type = 0;
    unsigned int spot = 0;
    int count = 0;
    int offset = 0;
};

struct VALayout
{
    std::vector<VAItem> layout;
    size_t stride = 0;
};


void VertexSetup::SetVertexLayout(const std::vector<VertexLayout>& layouts)
{
    m_vertLayout = layouts;

    // Buffer id (get from m_mesh) : corresponding layout
    std::unordered_map<size_t, VALayout> buffersLayout;
    for (const auto& layout: layouts)
    {
        VALayout& currentBL = buffersLayout[layout.id];

        GLenum type = 0;
        int count = 0, size = 0;

        // @formatter:off
        switch (layout.type)
        {
        case VarType::Char:   type = GL_BYTE;          count = 1;  size = sizeof(char);          break;
        case VarType::Int:    type = GL_INT;           count = 1;  size = sizeof(int);           break;
        case VarType::UInt:   type = GL_UNSIGNED_INT;  count = 1;  size = sizeof(unsigned int);  break;
        case VarType::Float:  type = GL_FLOAT;         count = 1;  size = sizeof(float);         break;
        case VarType::Double: type = GL_DOUBLE;        count = 1;  size = sizeof(double);        break;
        case VarType::Vec2:   type = GL_FLOAT;         count = 2;  size = sizeof(float) * 2;     break;
        case VarType::Vec3:   type = GL_FLOAT;         count = 3;  size = sizeof(float) * 3;     break;
        case VarType::Vec4:   type = GL_FLOAT;         count = 4;  size = sizeof(float) * 4;     break;
        case VarType::DVec2:  type = GL_DOUBLE;        count = 2;  size = sizeof(double) * 2;    break;
        case VarType::DVec3:  type = GL_DOUBLE;        count = 3;  size = sizeof(double) * 3;    break;
        case VarType::DVec4:  type = GL_DOUBLE;        count = 4;  size = sizeof(double) * 4;    break;
        case VarType::IVec2:  type = GL_INT;           count = 2;  size = sizeof(int) * 2;       break;
        case VarType::IVec3:  type = GL_INT;           count = 3;  size = sizeof(int) * 3;       break;
        case VarType::IVec4:  type = GL_INT;           count = 4;  size = sizeof(int) * 4;       break;
        default: std::cout << "[ERROR] Unknown vertex data type" << std::endl;                   break;
        }
        // @formatter:on

        currentBL.layout.emplace_back(type, layout.spot, count, currentBL.stride);
        currentBL.stride += size;
    }


    GLCall(glBindVertexArray(m_VA));

    for (const auto& [id, layout]: buffersLayout)
    {
        m_mesh->GetBuffer(id).Bind();

        for (const auto& item: layout.layout)
        {
            GLCall(glEnableVertexAttribArray(item.spot));

            auto offsetPtr = reinterpret_cast<void*>(item.offset);

            // @formatter:off
            switch (item.type)
            {
            case GL_DOUBLE:
                GLCall(glVertexAttribLPointer(item.spot, item.count, item.type, layout.stride, offsetPtr));
                break;

            case GL_BYTE:
            case GL_UNSIGNED_BYTE:
            case GL_SHORT:
            case GL_UNSIGNED_SHORT:
            case GL_INT:
            case GL_UNSIGNED_INT:
                GLCall(glVertexAttribIPointer(item.spot, item.count, item.type, layout.stride, offsetPtr));
                break;

            default:
                GLCall(glVertexAttribPointer(item.spot, item.count, item.type, GL_FALSE, layout.stride, offsetPtr));
            }
            // @formatter:on
        }
    }

    GLCall(glBindVertexArray(0));
}

void VertexSetup::SetInstanceLayout(std::vector<VertexLayout> layouts) {}


void VertexSetup::SetIndexBuffer(size_t indexBuffer) const
{
    Bind();
    m_mesh->GetBuffer(indexBuffer).Bind();

    Unbind();
    m_mesh->GetBuffer(indexBuffer).Unbind();
}

void VertexSetup::Bind() const
{
    GLCall(glBindVertexArray(m_VA));
}

void VertexSetup::Unbind()
{
    GLCall(glBindVertexArray(0));
}
