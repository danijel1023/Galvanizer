#include "BaseWindow.h"
#include "MainWindow.h"

#include "EventConfigurations.h"
#include "Utility.h"

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
        return parent->GetAbsPos() + p_virtualPos;

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
            winEvent.pos += p_pxPos;

            // Send to children and call the callback
            auto ret = GalvanizerObject::Dispatcher(event);

            // Remove my pos from absolute pos
            winEvent.pos -= p_pxPos;
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
                if (pos.x < ch->p_virtualPos.x)
                    continue;

                // If the mouse is RIGHT form the child window
                if (pos.x > ch->p_virtualPos.x + ch->p_virtualSize.x)
                    continue;

                // If the mouse is BELOW form the child window
                if (pos.y < ch->p_virtualPos.y)
                    continue;

                // If the mouse is ABOVE form the child window
                if (pos.y > ch->p_virtualPos.y + ch->p_virtualSize.y)
                    continue;

                pos -= ch->p_virtualPos;
                if (!ch->Dispatcher(event))
                    return 0;

                pos += ch->p_virtualPos;
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
                return target->Dispatcher(event);

            SetVirtualSize(winEvent.size);
            ResizeOutline(p_mainWindowRef->outlineThickness);

            p_mainWindowRef->PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);
            return 0;
        }

        case WindowMessage::Position:
        {
            SetVirtualPos(winEvent.pos);
            return 0;
        }

        case WindowMessage::ResizeRequest:
        {
            if (auto target = winEvent.objHndl.lock())
                p_parent.lock()->PostEvent(CreateWindowEvent<WindowMessage::Resize>(target, winEvent.size), p_weakSelf);

            return 0;
        }

        case WindowMessage::Scale:
        {
            SetVirtualPos(p_virtualPos);
            SetVirtualSize(p_virtualSize);
            return 0;
        }

        case WindowMessage::Render:
        {
            if (!p_mainWindowRef)
                return 0;

            if (!p_mainWindowRef->enableOutline || !m_renderOutline)
                return 0;

            auto scale = p_mainWindowRef->GetScale();

            Quad scaledQ0 = m_q0, scaledQ1 = m_q1, scaledQ2 = m_q2, scaledQ3 = m_q3;
            scaledQ0.size = {std::round(m_q0.size.x * scale.x), std::round(m_q0.size.y * scale.y)};
            scaledQ1.size = {std::round(m_q1.size.x * scale.x), std::round(m_q1.size.y * scale.y)};
            scaledQ2.size = {std::round(m_q2.size.x * scale.x), std::round(m_q2.size.y * scale.y)};
            scaledQ3.size = {std::round(m_q3.size.x * scale.x), std::round(m_q3.size.y * scale.y)};

            scaledQ2.pos = {0, std::round(p_pxSize.y) - scaledQ2.size.y};
            scaledQ3.pos = {std::round(p_pxSize.x) - scaledQ3.size.x, 0};

            p_mainWindowRef->renderer.AddQuad(scaledQ0);
            p_mainWindowRef->renderer.AddQuad(scaledQ1);
            p_mainWindowRef->renderer.AddQuad(scaledQ2);
            p_mainWindowRef->renderer.AddQuad(scaledQ3);
            p_mainWindowRef->renderer.Render();
            return 0;
        }

        default:
            return 0;
        }
    }

    else if (event->IsType<MouseEvent>())
    {
        auto& mouseEvent = static_cast<MouseEvent&>(*event);

        switch (mouseEvent.message)
        {
        case MouseMessage::Button:
        {
            if (mouseEvent.action == MouseAction::Press)
            {
                p_mainWindowRef->mouseFocusWin = std::static_pointer_cast<BaseWindow>(p_weakSelf.lock());
                p_mainWindowRef->keyboardFocus = p_weakSelf;
            }

            else
                p_mainWindowRef->mouseFocusWin.reset();

            return 0;
        }

        case MouseMessage::Move:
        {
            const auto self = std::static_pointer_cast<BaseWindow>(p_weakSelf.lock());
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

            if (!p_mainWindowRef->enableOutline)
                return 0;

            p_mainWindowRef->PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);
            return 0;
        }

        case MouseMessage::Leave:
        {
            m_renderOutline = false;

            if (!p_mainWindowRef->enableOutline)
                return 0;

            p_mainWindowRef->PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);
            return 0;
        }

        default:
            return 0;
        }
    }

    else if (event->IsType<ObjectEvent>())
    {
        auto& objEvent = static_cast<ObjectEvent&>(*event);

        switch (objEvent.message)
        {
        case ObjectMessage::Init:
        {
            m_q0.color = {224 / 255.0f, 134 / 255.0f, 60 / 255.0f, 1.0f};
            m_q1.color = {224 / 255.0f, 134 / 255.0f, 60 / 255.0f, 1.0f};
            m_q2.color = {224 / 255.0f, 134 / 255.0f, 60 / 255.0f, 1.0f};
            m_q3.color = {224 / 255.0f, 134 / 255.0f, 60 / 255.0f, 1.0f};

            ResizeOutline(p_mainWindowRef->outlineThickness);
            break;
        }

        default:
            break;
        }
    }

    return GalvanizerObject::Callback(event);
}


void BaseWindow::ResizeOutline(float virtualThickness)
{
    m_q0.size = {p_virtualSize.x, virtualThickness};
    m_q1.size = {virtualThickness, p_virtualSize.y};
    m_q2.size = {p_virtualSize.x, virtualThickness};
    m_q3.size = {virtualThickness, p_virtualSize.y};
}


void BaseWindow::SetVirtualPos(Vec2 pos)
{
    p_virtualPos = pos;
    //p_pxPos = Utility::PlatformScaleUp(pos, p_mainWindowRef->GetScale());
    p_pxPos = {pos.x * p_mainWindowRef->GetScale().x, pos.y * p_mainWindowRef->GetScale().y};
}

void BaseWindow::SetVirtualSize(Vec2 size)
{
    p_virtualSize = size;
    //p_pxSize = Utility::PlatformScaleUp(size, p_mainWindowRef->GetScale());
    p_pxSize = {size.x * p_mainWindowRef->GetScale().x, size.y * p_mainWindowRef->GetScale().y};
}

void BaseWindow::SetPxPos(Vec2 pos)
{
    p_pxPos = pos;
    //p_virtualPos = Utility::PlatformScaleDown(pos, p_mainWindowRef->GetScale());
    p_virtualPos = {pos.x / p_mainWindowRef->GetScale().x, pos.y / p_mainWindowRef->GetScale().y};
}

void BaseWindow::SetPxSize(Vec2 size)
{
    p_pxSize = size;
    //p_virtualSize = Utility::PlatformScaleDown(size, p_mainWindowRef->GetScale());
    p_virtualSize = {size.x / p_mainWindowRef->GetScale().x, size.y / p_mainWindowRef->GetScale().y};
}
