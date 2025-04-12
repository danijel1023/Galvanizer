#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "units.h"
#include "Application.h"

namespace Galvanizer::Utility
{
bool ValidateKeys(std::string_view target);
std::vector<std::string> ExtractKeys(std::string_view str);
const char* strBool(bool x);

template<class T>
void printList(const T& vec)
{
    std::cout << "[";

    bool firstPrint = true;
    for (const auto& elm: vec)
    {
        if (!firstPrint)
            std::cout << ", ";

        std::cout << elm;
        firstPrint = false;
    }

    std::cout << "]";
}


template<class T>
static T PlatformScaleDown(T vec, Vec2 scale)
{
    if (Application::GetPlatform() == Platform::X11 ||
        Application::GetPlatform() == Platform::Windows)
    {
        return T(vec.x / scale.x, vec.y / scale.y);
    }

    return vec;
}

template<class T>
static T PlatformScaleUp(T vec, Vec2 scale)
{
    if (Application::GetPlatform() == Platform::X11 ||
        Application::GetPlatform() == Platform::Windows)
    {
        return T(vec.x * scale.x, vec.y * scale.y);
    }

    return vec;
}
}
