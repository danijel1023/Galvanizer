#pragma once
#include "Buffer.h"
#include "VertexSetup.h"

#include <vector>


namespace Galvanizer::RenderAbstraction
{
class Mesh
{
public:
    size_t CreateVertexBuffer(size_t size, const void* data, BufferUsage usage = BufferUsage::Static);
    size_t CreateIndexBuffer(size_t size, const void* data, BufferUsage usage = BufferUsage::Static);

    // Must call Init and Exit explicitly!
    VertexSetup vertexSetup = VertexSetup(this);

    // No bounds check!!
    Buffer& GetBuffer(size_t id);

    [[nodiscard]] int IndexCount() const { return m_indexCount; }

private:
    std::vector<Buffer> m_buffers;
    size_t m_indexBuffer = 0;
    int m_indexCount = 0;
};
}
