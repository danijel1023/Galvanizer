#pragma once
#include "Quad.h"
#include "renderAbstraction/RenderObject.h"

namespace Galvanizer
{
enum class Rounding
{
    None, Round, Floor, Ceil, Trunc
};

class QuadRenderer
{
public:
    QuadRenderer();
    ~QuadRenderer();

    void Init(std::string_view vertSrc, std::string_view fragSrc);
    void Exit();

    void SetSpace(IVec2 absPos, IVec2 size);
    /*
    void StartRender();
    void EndRender();
    */

    void AddQuad(const Quad& quad, Vec2 scale = {1.0f, 1.0f},
                 Rounding sizeRoundType = Rounding::None, Rounding posRoundType = Rounding::None);

    void Clear();
    void Render();

    void AddTexture(const std::shared_ptr<QuadTex>& tex);
    void RemoveTexture(const std::shared_ptr<QuadTex>& tex);
    void UpdateTextures();

private:
    std::shared_ptr<RenderAbstraction::Program> m_prog;

    size_t m_posBuff;
    RenderAbstraction::RenderObject m_obj;

    std::vector<Quad> m_quadArr;
    std::vector<QuadVertex> m_quadVertices;
    std::vector<std::shared_ptr<RenderAbstraction::GPUTexture>> m_internalTextures;

    std::mutex m_texDBM;
    std::vector<std::shared_ptr<QuadTex>> m_texDB;
    std::vector<std::shared_ptr<QuadTex>> m_uploadTex, m_removeTex;
};
}
