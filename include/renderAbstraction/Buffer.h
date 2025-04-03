#pragma once
#include <cstdint>

namespace Galvanizer::RenderAbstraction
{
enum class BufferType { Unknown, Vertex, Index };

enum class BufferUsage { Unknown, Static, Dynamic };

class Buffer
{
public:
    Buffer(BufferType type, BufferUsage usage);
    Buffer(BufferType type, BufferUsage usage, int64_t size, const void* data);

    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    Buffer(const Buffer& other) = delete;
    Buffer& operator=(const Buffer& other) = delete;

    ~Buffer();

    void Bind() const;
    void Unbind() const;

    void UpdateData(int64_t offset, int64_t size, const void* data) const;
    [[nodiscard]] BufferType GetType() const { return m_type; }

private:
    BufferType m_type;
    unsigned int m_id = 0;
};
}
