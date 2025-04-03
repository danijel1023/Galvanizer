#pragma once
#include <memory>
#include <renderAbstraction/RenderObject.h>

#include "units.h"


namespace Galvanizer
{
struct QuadTex
{
    // Function to free the pixel data on VRAM texture copy
    void (*deleter)(uint8_t* data) = &Utility::RenderAbstraction::DestroyTexture;
    uint8_t* data = nullptr;

    RenderAbstraction::TextureSpecs specs;

    // Might not need smart ptr here
    std::shared_ptr<RenderAbstraction::GPUTexture> gpuTex;
    std::mutex texM;
};


enum class QuadType : int
{
    Rectangle, Circle, MSDF, Mask
};

struct Quad
{
    bool active = true;
    IVec2 size, pos;
    Vec2 rotPoint;

    float innerDiameter = 0.0;
    float rotation = 0.0f;

    IVec2 texSize, texPos;
    std::weak_ptr<QuadTex> texture;

    Vec4 color;

    QuadType type = QuadType::Rectangle;
};

struct QuadVertex
{
    int type = 0;
    IVec2 pos;

    Vec4 color;

    Vec2 rotationCenter;
    Vec2 texCoords;

    int texSlot = -1;
    float rotation = 0;
    float innerDiameter = 0;

    IVec4 posSize;
};
}
