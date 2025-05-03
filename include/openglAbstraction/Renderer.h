#pragma once
#include "InstanceArray.h"
#include "RenderObject.h"

namespace Galvanizer::OpenglAbstraction
{
class Renderer
{
public:
    static void Draw(RenderObject& obj);
    static void Draw(RenderObject& obj, InstanceArray& instanceArr);

    static void Clear();
    static void ClearColor(Vec4 color);
    static void EnableBlending();
};
}
