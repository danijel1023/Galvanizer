#include <iostream>

#include "SimpleChild.h"
#include "MainWindow.h"
#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;

namespace
{
struct SimpleChild_Shared : SimpleChild
{
    template<typename... Args>
    SimpleChild_Shared(Args&&... args): SimpleChild(std::forward<Args>(args)...) {}
};
}

std::shared_ptr<GObj> SimpleChild::factory(std::string_view name, const std::weak_ptr<GObj>& parent, Factory* originFac)
{
    return std::make_shared<SimpleChild_Shared>(name, parent, originFac);
}

SimpleChild::SimpleChild(const std::string_view name, const std::weak_ptr<GObj>& parent, Factory* originFac)
    : BaseWindow(name, parent, originFac) {}


SimpleChild::~SimpleChild() = default;

uintptr_t SimpleChild::Callback(const std::shared_ptr<Event>& event)
{
    if (event->IsType<ObjectEvent>())
    {
        auto objEvent = static_cast<ObjectEvent&>(*event);

        switch (objEvent.message)
        {
        case ObjectMessage::Init:
        {
            p_pos = {50, 50};
            p_size = {100, 200};

            m_bkg.color = {117.0 / 255, 137.0 / 255, 129.0 / 255, 1.0};
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
            if (m_tracking)
            {
                // Keep the distance from mouse and the lower left corner of the BaseWindow, the same.
                p_pos += mouseEvent.pos - m_diff;
                p_mainWindowRef->PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);
            }


            //std::cout << "[DEBUG] Child mouse move: [" << mouseEvent.pos.x << ", " << mouseEvent.pos.y << "]" <<
            //        std::endl;
            break;
        }

        case MouseMessage::Button:
        {
            std::cout << "[DEBUG] Child button! [" << mouseEvent.pos.x << ", " << mouseEvent.pos.y << "]" <<
                    std::endl;

            if (mouseEvent.action == MouseAction::Press)
                m_tracking = true;
            else
                m_tracking = false;

            m_diff = mouseEvent.pos;

            break;
        }

        default:
            break;
        }
    }

    return BaseWindow::Callback(event);
}
