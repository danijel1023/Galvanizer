#include "openglAbstraction/Technique.h"

#include "glad/glad.h"
#include "GLErr.inl"


using namespace Galvanizer::OpenglAbstraction;

void Technique::UseTexture(const std::weak_ptr<GPUTexture>& texture, const std::string& uniform)
{
    m_textures[uniform] = texture;
}


void Technique::RemoveTexture(const std::string& uniform)
{
    m_textures.erase(uniform);
}


void Technique::BindTextures()
{
    int i = 0;
    for (auto& [uniform, texture]: m_textures)
    {
        if (auto tex = texture.lock())
        {
            GLCall(glBindTexture(GL_TEXTURE_2D, tex->GetID()));
            GLCall(glActiveTexture(GL_TEXTURE0 + i));

            if (!program.lock()->SetUniformI(uniform, i))
                std::cout << "[ERROR] Couldn't set texture uniform: " << uniform << std::endl;

            i++;
        }
    }
}
