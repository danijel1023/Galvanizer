#pragma once
#include "units.h"
#include <vector>

namespace Galvanizer::RenderAbstraction
{
struct VertexLayout
{
    size_t id;
    VarType type;
    int spot;
};

class Mesh;

class VertexSetup
{
public:
    explicit VertexSetup(Mesh* mesh);

    void Init();
    void Exit();

    void SetVertexLayout(const std::vector<VertexLayout>& layouts);
    void SetInstanceLayout(std::vector<VertexLayout> layouts);

    void SetIndexBuffer(size_t indexBuffer) const;

    void Bind() const;
    static void Unbind();

private:
    std::vector<VertexLayout> m_vertLayout;
    std::vector<VertexLayout> m_InstLayout;

    Mesh* m_mesh;
    unsigned int m_VA = 0;
};
}
