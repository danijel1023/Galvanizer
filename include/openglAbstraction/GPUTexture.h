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

enum class TextureFormat { R = 1, RG, RGB, BGR, RGBA, BGRA };

inline int GetTextureChannels(TextureFormat format)
{
    // @formatter:off
    switch (format)
    {
    case TextureFormat::R:    return 1;
    case TextureFormat::RG:   return 2;
    case TextureFormat::RGB:  return 3;
    case TextureFormat::BGR:  return 3;
    case TextureFormat::RGBA: return 4;
    case TextureFormat::BGRA: return 4;
    default: return 0;
    }
    // @formatter:on
}

struct TextureSpecs
{
    TextureFormat format = {};
    IVec2 size;

    TextureWrap horizontal = TextureWrap::Repeat;
    TextureWrap vertical = TextureWrap::Repeat;

    // magnify can't use Mipmap texture filtering
    TextureFiltering magnify = TextureFiltering::Linear;
    TextureFiltering minimize = TextureFiltering::LinearMipmap_Linear;

    // Remember to set minimize filtering to non-mipmap one if disabling mipmaps
    bool generateMipmaps = true;
};



struct GPUTexture
{
    // Doesn't own data - only upload to the GPU
    GPUTexture(const void* data, const TextureSpecs& specs);

    GPUTexture(const GPUTexture&) = delete;
    GPUTexture& operator=(const GPUTexture&) = delete;
    GPUTexture(GPUTexture&& other) noexcept;
    GPUTexture& operator=(GPUTexture&& other) noexcept;

    ~GPUTexture();

    // Does not resize the texture - if you need a different size, make a new texture
    void UpdateData(const void* data, IVec2 offset, IVec2 size);

    [[nodiscard]] unsigned int GetID() const { return m_id; }

private:
    TextureSpecs m_specs;
    unsigned int m_id = 0;
};
}

namespace Utility::OpenglAbstraction
{
void* LoadTexture(const std::filesystem::path& path, Galvanizer::OpenglAbstraction::TextureSpecs* specs);
void DestroyTexture(void* data);
}
}
