#pragma once
#include "units.h"

#include <filesystem>

namespace Galvanizer
{
namespace OpenglAbstraction
{
enum class TextureWrap { Repeat, Mirror, ClampEdge, ClampBorder };

enum class TextureFiltering
{
    Nearest, Linear,
    NearestMipmap_Nearset, NearestMipmap_Linear,
    LinearMipmap_Nearset, LinearMipmap_Linear
};


struct TextureSpecs
{
    int channels = 0;
    Vec2 size;

    TextureWrap horizontal = TextureWrap::Repeat;
    TextureWrap vertical = TextureWrap::Repeat;

    // magnify can't use Mipmap texture filtering
    TextureFiltering magnify = TextureFiltering::Linear;
    TextureFiltering minimize = TextureFiltering::LinearMipmap_Linear;
};



struct GPUTexture
{
    // Doesn't own data - only upload to the GPU
    GPUTexture(const unsigned char* data, const TextureSpecs& specs);

    GPUTexture(const GPUTexture&) = delete;
    GPUTexture& operator=(const GPUTexture&) = delete;
    GPUTexture(GPUTexture&& other) noexcept;
    GPUTexture& operator=(GPUTexture&& other) noexcept;

    ~GPUTexture();

    [[nodiscard]] unsigned int GetID() const { return m_id; }

private:
    unsigned int m_id = 0;
};
}

namespace Utility::OpenglAbstraction
{
uint8_t* LoadTexture(const std::filesystem::path& path, Galvanizer::OpenglAbstraction::TextureSpecs* specs);
void DestroyTexture(uint8_t* data);
}
}
