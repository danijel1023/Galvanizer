#pragma once

#include "units.h"
#include "Events.h"
#include "GalvanizerObject.h"

#include "Quad.h"

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

    [[nodiscard]] Vec2 GetAbsPos() const;

    uintptr_t Callback(const std::shared_ptr<Event>& event) override;
    uintptr_t Dispatcher(const std::shared_ptr<Event>& event) override;

protected:
    BaseWindow(std::string_view name, const std::weak_ptr<GObj>& parent, Factory* originFac);

protected:
    WinHNDL p_mainWindowRef = nullptr; // TODO make this also smart pointer... maybe?
    Vec2 p_size, p_pos;
    bool p_overlay = false;

    bool p_isMainWin = false;

private:
    Quad m_q0, m_q1, m_q2, m_q3;
    bool m_renderOutline = false;
};
}
