#pragma once
#include "Technique.h"
#include "Mesh.h"

namespace Galvanizer::RenderAbstraction
{
class RenderObject
{
public:
    Technique technique;
    Mesh mesh;
};
}
