#include <iostream>

#include "SimpleChild2.h"
#include "MainWindow.h"
#include "EventConfigurations.h"

using namespace Galvanizer;

namespace
{
struct SimpleChild2_Shared : SimpleChild2
{
    template<typename... Args>
    SimpleChild2_Shared(Args&&... args): SimpleChild2(std::forward<Args>(args)...) {}
};
}

std::shared_ptr<GObj> SimpleChild2::factory(std::string_view name, const std::weak_ptr<GObj>& parent,
                                            Factory* originFac)
{
    return std::make_shared<SimpleChild2_Shared>(name, parent, originFac);
}

SimpleChild2::SimpleChild2(const std::string_view name, const std::weak_ptr<GObj>& parent, Factory* originFac)
    : BaseWindow(name, parent, originFac) {}


SimpleChild2::~SimpleChild2() = default;

uintptr_t SimpleChild2::Callback(const std::shared_ptr<Event>& event)
{
    if (event->IsType<ObjectEvent>())
    {
        auto& objEvent = static_cast<ObjectEvent&>(*event);

        switch (objEvent.message)
        {
        case ObjectMessage::Init:
        {
            SetVirtualPos({20, 100});
            SetVirtualSize({40, 40});

            m_bkg.color = {212.0 / 255, 226.0 / 255, 83.0 / 255, 1.0};
            m_bkg.size = p_virtualSize;
            break;
        }

        default:
            break;
        }
    }

    else if (event->IsType<WindowEvent>())
    {
        auto& winEvent = static_cast<WindowEvent&>(*event);

        switch (winEvent.message)
        {
        case WindowMessage::Render:
        {
            p_mainWindowRef->renderer.SetSpace(Utility::Round(winEvent.pos), Utility::Round(p_pxSize));
            p_mainWindowRef->renderer.AddQuad(m_bkg, p_mainWindowRef->GetScale());
            p_mainWindowRef->renderer.Render();
            break;
        }

        default:
            break;
        }
    }

    return BaseWindow::Callback(event);
}
