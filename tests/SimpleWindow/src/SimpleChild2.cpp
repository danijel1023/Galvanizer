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
        auto objEvent = static_cast<ObjectEvent&>(*event);

        switch (objEvent.message)
        {
        case ObjectMessage::Init:
        {
            p_pos = {20, 100};
            p_size = {40, 40};

            m_bkg.color = {212.0 / 255, 226.0 / 255, 83.0 / 255, 1.0};
            m_bkg.size = p_size;
            break;
        }

        default:
            break;
        }
    }

    else if (event->IsType<WindowEvent>())
    {
        auto winEvent = static_cast<WindowEvent&>(*event);

        switch (winEvent.message)
        {
        case WindowMessage::Render:
        {
            p_mainWindowRef->renderer.SetSpace(winEvent.pos, p_size);
            p_mainWindowRef->renderer.AddQuad(m_bkg);
            p_mainWindowRef->renderer.Render();
            break;
        }

        default:
            break;
        }
    }

    else if (event->IsType<MouseEvent>())
    {
        auto mouseEvent = static_cast<MouseEvent&>(*event);

        switch (mouseEvent.message)
        {
        case MouseMessage::Move:
        {
            //std::cout << "[DEBUG] Child2 mouse move: [" << mouseEvent.pos.x << ", " << mouseEvent.pos.y << "]" <<
            //        std::endl;
            break;
        }

        case MouseMessage::Button:
        {
            std::cout << "[DEBUG] Child2 press! [" << mouseEvent.pos.x << ", " << mouseEvent.pos.y << "]" <<
                    std::endl;
            break;
        }

        default:
            break;
        }
    }

    return BaseWindow::Callback(event);
}
