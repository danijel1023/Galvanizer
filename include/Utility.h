#pragma once

#include <vector>
#include <string>
#include <iostream>


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
}
