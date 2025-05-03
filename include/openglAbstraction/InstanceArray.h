#pragma once
#include "Buffer.h"

#include <cstddef>

namespace Galvanizer::OpenglAbstraction
{
class InstanceArray
{
public:
    size_t CreateBuffer(BufferType type, size_t size, const void* data, BufferUsage usage);
};
}
