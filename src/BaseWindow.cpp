#include "BaseWindow.h"
#include "MainWindow.h"

#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;

namespace
{
struct BaseWindow_Shared : BaseWindow
{
    template<typename... Args>
    BaseWindow_Shared(Args&&... args): BaseWindow(std::forward<Args>(args)...) {}
};
}

std::shared_ptr<GObj> BaseWindow::factory(std::string_view name, const std::weak_ptr<GObj>& parent, Factory* originFac)
{
    return std::make_shared<BaseWindow_Shared>(name, parent, originFac);
}


BaseWindow::BaseWindow(const std::string_view name, const std::weak_ptr<GObj>& parent, Factory* originFac)
    : GalvanizerObject(name, parent, originFac)
{
    const auto lockedParent = parent.lock();

    if (const auto winParent = dynamic_cast<BWinHNDL>(lockedParent.get()))
        p_mainWindowRef = winParent->p_mainWindowRef;
}

BaseWindow::~BaseWindow() = default;


Vec2 BaseWindow::GetAbsPos() const
{
    if (p_isMainWin)
        return {};

    if (const auto parent = std::dynamic_pointer_cast<BaseWindow>(p_parent.lock()))
        return parent->GetAbsPos() + p_pos;

    return {};
}



uintptr_t BaseWindow::Dispatcher(const std::shared_ptr<Event>& event)
{
    if (event->IsType<WindowEvent>())
    {
        auto& winEvent = static_cast<WindowEvent&>(*event);
        switch (winEvent.message)
        {
        case WindowMessage::Render:
        {
            // Add my pos to absolute pos
            winEvent.pos += p_pos;

            // Send to children and call the callback
            auto ret = GalvanizerObject::Dispatcher(event);

            // Remove my pos from absolute pos
            winEvent.pos -= p_pos;
            return ret;
        }

        default:
            break;
        }
    }

    else if (event->IsType<MouseEvent>())
    {
        auto& mouseEvent = static_cast<MouseEvent&>(*event);

        auto& pos = mouseEvent.pos;
        for (int i = 0; i < 2; i++)
        {
            for (auto& GObjCh: p_children)
            {
                auto ch = std::static_pointer_cast<BaseWindow>(GObjCh);
                if (!ch)
                    continue;

                if (ch->p_isMainWin)
                    continue;

                // First run on the overlay, then on normal
                if (ch->p_overlay == i)
                    continue;

                // If the mouse is LEFT form the child window
                if (pos.x < ch->p_pos.x)
                    continue;

                // If the mouse is RIGHT form the child window
                if (pos.x > ch->p_pos.x + ch->p_size.x)
                    continue;

                // If the mouse is BELOW form the child window
                if (pos.y < ch->p_pos.y)
                    continue;

                // If the mouse is ABOVE form the child window
                if (pos.y > ch->p_pos.y + ch->p_size.y)
                    continue;

                pos -= ch->p_pos;
                if (!ch->Dispatcher(event))
                    return 0;

                pos += ch->p_pos;
            }
        }
    }

    return GalvanizerObject::Dispatcher(event);
}


uintptr_t BaseWindow::Callback(const std::shared_ptr<Event>& event)
{
    if (event->IsType<WindowEvent>())
    {
        auto& winEvent = static_cast<WindowEvent&>(*event);

        switch (winEvent.message)
        {
        case WindowMessage::Refresh:
        {
            PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);
            return 0;
        }

        case WindowMessage::Resize:
        {
            auto target = winEvent.objHndl.lock();
            if (target && target != p_weakSelf.lock())
            {
                target->PostEvent(CreateWindowEvent<WindowMessage::Resize>(winEvent.objHndl, winEvent.size),
                                  p_weakSelf);
                return 0;
            }

            p_size = winEvent.size;
            PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);

            m_q0.pos = {0, 0};
            m_q1.pos = {0, 0};
            m_q2.pos = {0, p_size.y - 2};
            m_q3.pos = {p_size.x - 2, 0};

            m_q0.size = {p_size.x, 2};
            m_q1.size = {2, p_size.y};
            m_q2.size = {p_size.x, 2};
            m_q3.size = {2, p_size.y};

            return 0;
        }

        case WindowMessage::Position:
        {
            p_pos = winEvent.pos;
            return 0;
        }

        case WindowMessage::ResizeRequest:
        {
            if (auto target = winEvent.objHndl.lock())
                p_parent.lock()->PostEvent(CreateWindowEvent<WindowMessage::Resize>(target, winEvent.size), p_weakSelf);

            return 0;
        }

        case WindowMessage::Render:
        {
            if (!p_mainWindowRef)
                return 0;

            if (!m_renderOutline)
                return 0;

            p_mainWindowRef->renderer.AddQuad(m_q0);
            p_mainWindowRef->renderer.AddQuad(m_q1);
            p_mainWindowRef->renderer.AddQuad(m_q2);
            p_mainWindowRef->renderer.AddQuad(m_q3);
            p_mainWindowRef->renderer.Render();
            return 0;
        }

        default:
            return 0;
        }
    }

    else if (event->IsType<MouseEvent>())
    {
        auto mouseEvent = static_cast<MouseEvent&>(*event);

        switch (mouseEvent.message)
        {
        case MouseMessage::Button:
        {
            if (mouseEvent.action == MouseAction::Press)
                p_mainWindowRef->mouseFocusWin = std::static_pointer_cast<BaseWindow>(p_weakSelf.lock());
            else
                p_mainWindowRef->mouseFocusWin.reset();

            return 0;
        }

        case MouseMessage::Move:
        {
            auto self = std::static_pointer_cast<BaseWindow>(p_weakSelf.lock());
            auto winUnderCursor = p_mainWindowRef->winUnderCursor.lock();
            if (winUnderCursor != self)
            {
                if (winUnderCursor)
                    winUnderCursor->Callback(CreateMouseEvent<MouseMessage::Leave>());

                Callback(CreateMouseEvent<MouseMessage::Enter>());
                p_mainWindowRef->winUnderCursor = self;
            }

            return 0;
        }

        case MouseMessage::Enter:
        {
            m_renderOutline = true;
            p_mainWindowRef->PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);
            return 0;
        }

        case MouseMessage::Leave:
        {
            m_renderOutline = false;
            p_mainWindowRef->PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);
            return 0;
        }

        default:
            return 0;
        }
    }

    else if (event->IsType<ObjectEvent>())
    {
        auto objEvent = static_cast<ObjectEvent&>(*event);

        switch (objEvent.message)
        {
        case ObjectMessage::Init:
        {
            m_q0.pos = {0, 0};
            m_q1.pos = {0, 0};
            m_q2.pos = {0, p_size.y - 2};
            m_q3.pos = {p_size.x - 2, 0};

            m_q0.size = {p_size.x, 2};
            m_q1.size = {2, p_size.y};
            m_q2.size = {p_size.x, 2};
            m_q3.size = {2, p_size.y};

            m_q0.color = {224.0f / 255, 134.0f / 255, 60.0f / 255, 1.0f};
            m_q1.color = {224.0f / 255, 134.0f / 255, 60.0f / 255, 1.0f};
            m_q2.color = {224.0f / 255, 134.0f / 255, 60.0f / 255, 1.0f};
            m_q3.color = {224.0f / 255, 134.0f / 255, 60.0f / 255, 1.0f};
            break;
        }

        default:
            break;
        }
    }

    return GalvanizerObject::Callback(event);
}
