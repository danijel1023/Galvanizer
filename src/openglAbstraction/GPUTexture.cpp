#include "openglAbstraction/GPUTexture.h"

#include "stb/stb_image.h"
#include "glad/glad.h"
#include "GLErr.inl"


using namespace Galvanizer::OpenglAbstraction;

static GLint GetGLTextureWrap(const TextureWrap wrap)
{
    switch (wrap)
    {
    case TextureWrap::Repeat:
        return GL_REPEAT;
    case TextureWrap::Mirror:
        return GL_MIRRORED_REPEAT;

    case TextureWrap::ClampBorder:
        return GL_CLAMP_TO_BORDER;
    case TextureWrap::ClampEdge:
        return GL_CLAMP_TO_EDGE;

    default:
        std::cout << "[ERROR OpenGL] Unknown TextureWrap" << std::endl;
        return 0;
    }
}


static GLint GetGLTextureFiltering(const TextureFiltering filtering)
{
    switch (filtering)
    {
    case TextureFiltering::Linear:
        return GL_LINEAR;
    case TextureFiltering::Nearest:
        return GL_NEAREST;

    case TextureFiltering::LinearMipmap_Linear:
        return GL_LINEAR_MIPMAP_LINEAR;
    case TextureFiltering::LinearMipmap_Nearset:
        return GL_NEAREST_MIPMAP_LINEAR;

    case TextureFiltering::NearestMipmap_Linear:
        return GL_LINEAR_MIPMAP_NEAREST;
    case TextureFiltering::NearestMipmap_Nearset:
        return GL_NEAREST_MIPMAP_NEAREST;

    default:
        std::cout << "[ERROR OpenGL] Unknown TextureFiltering" << std::endl;
        return 0;
    }
}

GPUTexture::GPUTexture(const unsigned char* data, const TextureSpecs& specs)
{
    GLCall(glGenTextures(1, &m_id));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_id));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetGLTextureWrap(specs.horizontal)));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetGLTextureWrap(specs.vertical)));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGLTextureFiltering(specs.minimize)));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGLTextureFiltering(specs.magnify)));

    GLenum dataFormat;
    if (specs.channels == 4)
        dataFormat = GL_RGBA;
    else
        dataFormat = GL_RGB;

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, specs.size.x, specs.size.y, 0, dataFormat, GL_UNSIGNED_BYTE, data));
    GLCall(glGenerateMipmap(GL_TEXTURE_2D));
}



GPUTexture::GPUTexture(GPUTexture&& other) noexcept
    : m_id(other.m_id)
{
    other.m_id = 0;
}

GPUTexture& GPUTexture::operator=(GPUTexture&& other) noexcept
{
    if (this == &other)
        return *this;

    m_id = other.m_id;
    other.m_id = 0;

    return *this;
}

GPUTexture::~GPUTexture()
{
    GLCall(glDeleteTextures(1, &m_id));
}


uint8_t* Galvanizer::Utility::OpenglAbstraction::LoadTexture(const std::filesystem::path& path, TextureSpecs* specs)
{
    stbi_set_flip_vertically_on_load(true);

    int x, y;
    auto data = stbi_load(path.c_str(), &x, &y, &specs->channels, 0);
    specs->size = Vec2(x, y);

    if (!data)
        std::cout << "[ERROR] Failed to load texture on path: " << path << std::endl;

    return data;
}

void Galvanizer::Utility::OpenglAbstraction::DestroyTexture(uint8_t* data)
{
    stbi_image_free(data);
}
