#pragma once

namespace Galvanizer
{
struct Vec2
{
    float x = 0, y = 0;

    template<class T>
    Vec2 operator+(const T& second) const
    {
        return Vec2(x + second.x, y + second.y);
    }

    Vec2 operator+(const float& second) const
    {
        return Vec2(x + second, y + second);
    }

    Vec2 operator*(const float& second) const
    {
        return Vec2(x * second, y * second);
    }

    template<class T>
    Vec2 operator-(const T& second) const
    {
        return Vec2(x - second.x, y - second.y);
    }

    template<class T>
    Vec2 operator+=(const T& second)
    {
        x += second.x;
        y += second.y;
        return *this;
    }

    template<class T>
    Vec2 operator-=(const T& second)
    {
        x -= second.x;
        y -= second.y;
        return *this;
    }
};

struct Vec3
{
    float x = 0, y = 0, z = 0;
};

struct Vec4
{
    float x = 0, y = 0, z = 0, a = 0;
};

struct DVec2
{
    double x = 0, y = 0;
};

struct DVec3
{
    double x = 0, y = 0, z = 0;
};

struct DVec4
{
    double x = 0, y = 0, z = 0, a = 0;
};


struct IVec2
{
    int x = 0, y = 0;

    IVec2 operator+(const IVec2& second) const
    {
        return {x + second.x, y + second.y};
    }

    IVec2 operator-(const IVec2& second) const
    {
        return {x - second.x, y - second.y};
    }

    IVec2 operator+=(const IVec2& second)
    {
        x += second.x;
        y += second.y;
        return *this;
    }

    IVec2 operator-=(const IVec2& second)
    {
        x -= second.x;
        y -= second.y;
        return *this;
    }
};

struct IVec3
{
    int x = 0, y = 0, z = 0;
};

struct IVec4
{
    int x = 0, y = 0, z = 0, a = 0;
};

enum class VarType
{
    Char, Int, UInt, Float, Double,

    Vec2, Vec3, Vec4,
    DVec2, DVec3, DVec4,
    IVec2, IVec3, IVec4
};
}
