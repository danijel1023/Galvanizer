#include <iostream>

#include "openglAbstraction/Buffer.h"

#include "glad/glad.h"
#include "GLErr.inl"


using namespace Galvanizer::OpenglAbstraction;

static GLenum GetGLType(BufferType type)
{
    switch (type)
    {
    case BufferType::Index:
        return GL_ELEMENT_ARRAY_BUFFER;

    case BufferType::Vertex:
        return GL_ARRAY_BUFFER;

    default:
        std::cout << "[OpenGL ERROR - Buffer]: Unknown target (type)" << std::endl;
        return 0;
    }
}

static GLenum GetGLUsage(BufferUsage usage)
{
    switch (usage)
    {
    case BufferUsage::Dynamic:
        return GL_DYNAMIC_DRAW;

    case BufferUsage::Static:
        return GL_STATIC_DRAW;

    default:
        std::cout << "[OpenGL ERROR - Buffer]: Unknown usage" << std::endl;
        return 0;
    }
}


Buffer::Buffer(BufferType type, BufferUsage usage)
    : m_type(type)
{
    GLCall(glGenBuffers(1, &m_id));

    Bind();
    GLCall(glBufferData(GetGLType(m_type), 0, nullptr, GetGLUsage(usage)));
}

Buffer::Buffer(BufferType type, BufferUsage usage, int64_t size, const void* data)
    : m_type(type)
{
    GLCall(glGenBuffers(1, &m_id));

    Bind();
    GLCall(glBufferData(GetGLType(m_type), size, data, GetGLUsage(usage)));
}


Buffer::Buffer(Buffer&& other) noexcept
    : m_type(other.m_type),
      m_id(other.m_id)
{
    other.m_id = 0;
}


Buffer& Buffer::operator=(Buffer&& other) noexcept
{
    if (this == &other)
        return *this;

    m_type = other.m_type;
    m_id = other.m_id;

    other.m_id = 0;
    return *this;
}

Buffer::~Buffer()
{
    GLCall(glDeleteBuffers(1, &m_id));
}


void Buffer::Bind() const
{
    GLCall(glBindBuffer(GetGLType(m_type), m_id));
}

void Buffer::Unbind() const
{
    GLCall(glBindBuffer(GetGLType(m_type), 0));
}


void Buffer::UpdateData(int64_t offset, int64_t size, const void* data) const
{
    Bind();
    GLCall(glBufferSubData(GetGLType(m_type), offset, size, data));
}
