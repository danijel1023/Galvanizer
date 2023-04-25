#pragma once

#include <string>
#include "units.h"

namespace Galvanizer
{

class BaseWindow;

using BWinHNDL = BaseWindow*;
using WinFactoryPtr = BWinHNDL(*)();


class BaseWindow
{
public:
    BaseWindow(BWinHNDL parent, const std::string& name, Vec2 size, Vec2 pos);
    BaseWindow(const std::string& name, Vec2 size, Vec2 pos);
    virtual ~BaseWindow();

    std::string getPath();

    inline const std::string& getName() { return p_name; }

public:
    Vec2 size, pos;

protected:
    std::string p_name;
    BWinHNDL p_parent = nullptr;
    BWinHNDL p_mainWindow = nullptr;

private:

    friend class Application;
};

}
