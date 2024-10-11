#pragma once

#include <string>
#include <vector>

#include "units.h"
#include "Events.h"
#include "GalvanizerObject.h"

namespace Galvanizer
{

class MainWindow;

class BaseWindow;

using WinHNDL = MainWindow*;
using BWinHNDL = BaseWindow*;


class BaseWindow
    : public GalvanizerObject
{
public:
    static GObjHNDL factory(std::string_view name, GObjHNDL parent, Factory* originFac);
    ~BaseWindow() override;


    uintptr_t Callback(std::shared_ptr<Event> event) override;
    uintptr_t Dispatcher(std::shared_ptr<Event> event) override;

public:
    Vec2 size, pos;

protected:
    BaseWindow(std::string_view name, GObjHNDL parent, Factory* originFac);

protected:
    WinHNDL p_mainWindow = nullptr;
};

}
