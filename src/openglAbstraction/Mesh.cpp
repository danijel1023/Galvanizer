#include "openglAbstraction/Mesh.h"


using namespace Galvanizer::RenderAbstraction;

size_t Mesh::CreateVertexBuffer(size_t size, const void* data, BufferUsage usage)
{
    m_buffers.emplace_back(BufferType::Vertex, usage, size, data);
    return m_buffers.size() - 1;
}

size_t Mesh::CreateIndexBuffer(size_t size, const void* data, BufferUsage usage)
{
    if (m_indexBuffer)
        return 0;

    m_buffers.emplace_back(BufferType::Index, usage, size, data);
    m_indexBuffer = m_buffers.size() - 1;

    m_indexCount = size / sizeof(unsigned int);
    return m_indexBuffer;
}

Buffer& Mesh::GetBuffer(size_t id)
{
    return m_buffers[id];
}
