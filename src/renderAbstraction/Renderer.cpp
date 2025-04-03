#include "renderAbstraction/Renderer.h"

#include "glad/glad.h"
#include "renderAbstraction/GLErr.inl"


using namespace Galvanizer::RenderAbstraction;

void Renderer::Draw(RenderObject& obj)
{
    obj.mesh.vertexSetup.Bind();

    obj.technique.program.lock()->Bind();
    obj.technique.BindTextures();


    GLCall(glDrawElements(GL_TRIANGLES, obj.mesh.IndexCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(RenderObject& obj, InstanceArray& instanceArr) {}

void Renderer::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::ClearColor(Vec4 color)
{
    glClearColor(color.x, color.y, color.z, color.a);
}

void Renderer::EnableBlending()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
