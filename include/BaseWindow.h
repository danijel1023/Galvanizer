#pragma once

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
    static std::shared_ptr<GObj> factory(std::string_view name, const std::weak_ptr<GObj>& parent, Factory* originFac);
    ~BaseWindow() override;


    uintptr_t Callback(const std::shared_ptr<Event>& event) override;
    uintptr_t Dispatcher(const std::shared_ptr<Event>& event) override;

protected:
    BaseWindow(std::string_view name, const std::weak_ptr<GObj>& parent, Factory* originFac);

protected:
    WinHNDL p_mainWindow = nullptr; // TODO make this also smart pointer... maybe?
    IVec2 p_size, p_pos;
    bool p_overlay = false;
};
}
