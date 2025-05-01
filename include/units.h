#pragma once
#include <cmath>

namespace Galvanizer
{
struct IVec2;

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

    explicit operator IVec2() const;
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

    explicit operator Vec2() const;
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


inline Vec2::operator IVec2() const
{
    return IVec2(x, y);
}

inline IVec2::operator Vec2() const
{
    return Vec2(x, y);
}

namespace Utility
{
inline IVec2 Round(Vec2 vec)
{
    return IVec2(std::round(vec.x), std::round(vec.y));
}

inline IVec2 Floor(Vec2 vec)
{
    return IVec2(std::floor(vec.x), std::floor(vec.y));
}

inline IVec2 Ceil(Vec2 vec)
{
    return IVec2(std::ceil(vec.x), std::ceil(vec.y));
}

inline IVec2 Trunc(Vec2 vec)
{
    return IVec2(std::trunc(vec.x), std::trunc(vec.y));
}
}
}
