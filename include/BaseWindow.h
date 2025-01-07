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


class BaseWindow : public GalvanizerObject
{
public:
    static GObjHNDL factory(std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap);
    ~BaseWindow() override;


    uintptr_t Callback(const std::shared_ptr<Event>& event) override;
    uintptr_t Dispatcher(const std::shared_ptr<Event>& event) override;

public:
    Vec2 size, pos;

protected:
    BaseWindow(std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap);

protected:
    WinHNDL p_mainWindow = nullptr; // TODO make this also smart pointer
};
}
