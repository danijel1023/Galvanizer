#pragma once
#include "GPUTexture.h"
#include "Program.h"

#include <unordered_map>

namespace Galvanizer::RenderAbstraction
{
class Technique
{
public:
    std::weak_ptr<Program> program;

    void UseTexture(const std::weak_ptr<GPUTexture>& texture, const std::string& uniform);
    void RemoveTexture(const std::string& uniform);

    void BindTextures();

private:
    std::unordered_map<std::string, std::weak_ptr<GPUTexture>> m_textures;
};
}
