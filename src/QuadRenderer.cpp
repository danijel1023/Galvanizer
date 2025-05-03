#include "QuadRenderer.h"
#include "openglAbstraction/Renderer.h"
#include "Utility.h"

#include <iostream>
#include <array>

#include "glad/glad.h"

using namespace Galvanizer;

static unsigned int indices[] = {
    0, 1, 2,
    0, 2, 3,
};


QuadRenderer::QuadRenderer()
{
    m_quadArr.reserve(1024);
    m_quadVertices.reserve(m_quadArr.capacity() * 4);
}

QuadRenderer::~QuadRenderer() = default;


void QuadRenderer::Init(std::string_view vertSrc, std::string_view fragSrc)
{
    RenderAbstraction::Renderer::ClearColor({0.2f, 0.3f, 0.3f, 1.0f});
    RenderAbstraction::Renderer::EnableBlending();

    m_obj.mesh.vertexSetup.Init();

    m_prog = std::make_shared<RenderAbstraction::Program>();

    m_prog->CreateShader(vertSrc, RenderAbstraction::ShaderType::Vertex);
    m_prog->CreateShader(fragSrc, RenderAbstraction::ShaderType::Fragment);
    m_prog->CompileProgram();

    m_obj.technique.program = m_prog;


    auto& mesh = m_obj.mesh;
    m_posBuff = mesh.CreateVertexBuffer(sizeof(QuadVertex) * m_quadVertices.capacity(), nullptr,
                                        RenderAbstraction::BufferUsage::Dynamic);

    /*
     *      int type = 0;
     *      IVec2 pos;
     *      Vec4 color;
     *      Vec2 rotationCenter;
     *      Vec2 texCoords;
     *      int texID = 0;
     *      float rotation = 0;
     */

    mesh.vertexSetup.SetVertexLayout({
        {m_posBuff, VarType::Int, 0},
        {m_posBuff, VarType::Vec2, 1},
        {m_posBuff, VarType::Vec4, 2},
        {m_posBuff, VarType::Vec2, 3},
        {m_posBuff, VarType::Vec2, 4},
        {m_posBuff, VarType::Int, 5},
        {m_posBuff, VarType::Float, 6},
        {m_posBuff, VarType::Float, 7},
        {m_posBuff, VarType::Vec4, 8},
    });

    size_t indexBuff = mesh.CreateIndexBuffer(sizeof(indices), indices, RenderAbstraction::BufferUsage::Dynamic);
    mesh.vertexSetup.SetIndexBuffer(indexBuff);
}

void QuadRenderer::Exit()
{
    m_obj.mesh.vertexSetup.Exit();
    m_prog.reset();

    std::lock_guard lck(m_texDBM);
    m_texDB.clear();
    m_uploadTex.clear();
    m_removeTex.clear();
}

void QuadRenderer::Clear()
{
    RenderAbstraction::Renderer::Clear();
}

void QuadRenderer::Render()
{
    for (const auto& quad: m_quadArr)
    {
        std::array<QuadVertex, 4> vertices;

        int type = static_cast<int>(quad.type);

        int texSlot = -1;
        Vec2 nTexPos, nTexSize;
        if (auto tex = quad.texture.lock())
        {
            if (tex->gpuTex)
            {
                m_obj.technique.UseTexture(tex->gpuTex, "u_textures[0]");
                texSlot = 0;

                nTexPos = {quad.texPos.x / tex->specs.size.x, quad.texPos.y / tex->specs.size.y};
                nTexSize = {quad.texSize.x / tex->specs.size.x, quad.texSize.y / tex->specs.size.y};
            }
        }

        // @formatter:off

        vertices[0] = {type, {quad.pos.x,               quad.pos.y              }, quad.color, quad.rotPoint, {nTexPos.x,              nTexPos.y             }, texSlot, quad.rotation, quad.innerDiameter, {quad.pos.x, quad.pos.y, quad.size.x, quad.size.y}};
        vertices[1] = {type, {quad.pos.x + quad.size.x, quad.pos.y              }, quad.color, quad.rotPoint, {nTexPos.x + nTexSize.x, nTexPos.y             }, texSlot, quad.rotation, quad.innerDiameter, {quad.pos.x, quad.pos.y, quad.size.x, quad.size.y}};
        vertices[2] = {type, {quad.pos.x + quad.size.x, quad.pos.y + quad.size.y}, quad.color, quad.rotPoint, {nTexPos.x + nTexSize.x, nTexPos.y + nTexSize.y}, texSlot, quad.rotation, quad.innerDiameter, {quad.pos.x, quad.pos.y, quad.size.x, quad.size.y}};
        vertices[3] = {type, {quad.pos.x,               quad.pos.y + quad.size.y}, quad.color, quad.rotPoint, {nTexPos.x,              nTexPos.y + nTexSize.y}, texSlot, quad.rotation, quad.innerDiameter, {quad.pos.x, quad.pos.y, quad.size.x, quad.size.y}};

        // @formatter:on

        m_obj.mesh.GetBuffer(m_posBuff).UpdateData(0, sizeof(vertices), vertices.data());
        RenderAbstraction::Renderer::Draw(m_obj);
    }

    m_quadArr.clear();
}



void QuadRenderer::AddTexture(const std::shared_ptr<QuadTex>& tex)
{
    std::lock_guard lck(m_texDBM);
    m_uploadTex.emplace_back(tex);
}

void QuadRenderer::RemoveTexture(const std::shared_ptr<QuadTex>& tex)
{
    std::lock_guard lck(m_texDBM);
    m_removeTex.emplace_back(tex);
}


void QuadRenderer::UpdateTextures()
{
    std::lock_guard rootLck(m_texDBM);

    while (!m_uploadTex.empty())
    {
        if (auto tex = m_uploadTex[0])
        {
            std::lock_guard lck(tex->texM);
            if (tex->data && !tex->gpuTex)
            {
                tex->gpuTex = std::make_shared<RenderAbstraction::GPUTexture>(tex->data, tex->specs);

                if (tex->deleter)
                {
                    tex->deleter(tex->data);
                    tex->data = nullptr;
                }

                m_texDB.emplace_back(m_uploadTex[0]);
            }
        }

        m_uploadTex.erase(m_uploadTex.begin());
    }

    while (!m_removeTex.empty())
    {
        if (auto tex = m_removeTex[0])
        {
            std::lock_guard lck(tex->texM);

            if (tex->gpuTex)
            {
                // [TODO] This will almost certainly cause problems in the future - fix some sort of [CPU - GPU] id system thing ig... how??
                for (size_t i = 0; i < m_texDB.size(); i++)
                {
                    if (m_texDB[i] == m_removeTex[0])
                    {
                        m_texDB.erase(m_texDB.begin() + i);
                        break;
                    }
                }
            }

            tex->gpuTex.reset();
        }

        m_removeTex.erase(m_uploadTex.begin());
    }
}


void QuadRenderer::AddQuad(const Quad& quad_, Vec2 scale, Rounding sizeRoundType, Rounding posRoundType)
{
    m_quadArr.push_back(quad_);
    auto& quad = m_quadArr.back();

    quad.size = {quad.size.x * scale.x, quad.size.y * scale.y};
    quad.pos = {quad.pos.x * scale.x, quad.pos.y * scale.y};

    // @formatter:off
    switch (sizeRoundType)
    {
    case Rounding::Round: quad.size = Vec2(Utility::Round(quad.size)); break;
    case Rounding::Ceil:  quad.size = Vec2(Utility::Ceil(quad.size));  break;
    case Rounding::Floor: quad.size = Vec2(Utility::Floor(quad.size)); break;
    case Rounding::Trunc: quad.size = Vec2(Utility::Trunc(quad.size)); break;
    case Rounding::None:  break;
    }

    switch (posRoundType)
    {
    case Rounding::Round: quad.pos = Vec2(Utility::Round(quad.pos)); break;
    case Rounding::Ceil:  quad.pos = Vec2(Utility::Ceil(quad.pos));  break;
    case Rounding::Floor: quad.pos = Vec2(Utility::Floor(quad.pos)); break;
    case Rounding::Trunc: quad.pos = Vec2(Utility::Trunc(quad.pos)); break;
    case Rounding::None:  break;
    }
    // @formatter:on
}


void QuadRenderer::SetSpace(IVec2 absPos, IVec2 size)
{
    const auto prog = m_obj.technique.program.lock();

    glViewport(absPos.x, absPos.y, size.x, size.y);
    glScissor(absPos.x, absPos.y, size.x, size.y);

    if (!prog->SetUniformI("u_winSize", size.x, size.y))
        std::cout << "[ERROR] Couldn't set u_winSize uniform!" << std::endl;
}
