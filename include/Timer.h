#pragma once
#include <chrono>
#include <iostream>
#include <string>

namespace Galvanizer::Utility
{
struct Timer
{
    explicit Timer(bool print = true, const std::string& name = "")
        : print(print), name(name), tp(std::chrono::steady_clock::now()) {}

    ~Timer()
    {
        if (print)
            printDifference();
    }

    void printDifference() const
    {
        std::cout << "Elapsed time for " << name << ": " <<
                std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - tp) <<
                std::endl;
    }

    bool print;
    std::string name;
    std::chrono::time_point<std::chrono::steady_clock> tp;
};
}
