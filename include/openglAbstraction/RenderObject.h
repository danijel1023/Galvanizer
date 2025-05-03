#pragma once
#include "Technique.h"
#include "Mesh.h"

namespace Galvanizer::OpenglAbstraction
{
class RenderObject
{
public:
    Technique technique;
    Mesh mesh;
};
}
