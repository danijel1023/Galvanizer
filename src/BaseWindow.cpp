#include "BaseWindow.h"
#include "MainWindow.h"

#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;


GObjHNDL BaseWindow::factory(std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap)
{
    return new BaseWindow(name, parent, originFac, createdOnHeap);
}


BaseWindow::BaseWindow(const std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap)
    : GalvanizerObject(name, parent, originFac, createdOnHeap)
{
    const auto lockedParent = parent.lock();

    if (const auto winParent = dynamic_cast<BWinHNDL>(lockedParent.get()))
        p_mainWindow = winParent->p_mainWindow;
}

BaseWindow::~BaseWindow() = default;


uintptr_t BaseWindow::Dispatcher(const std::shared_ptr<Event>& event)
{
    if (event->IsType<WindowEvent>())
    {
        if (auto winEvent = static_cast<WindowEvent&>(*event); winEvent.message == WindowMessage::Render)
        {
            // Add my pos to absolute pos
            winEvent.pos += p_pos;

            // Send to children and call the callback
            auto ret = GalvanizerObject::Dispatcher(event);

            // Remove my pos from absolute pos
            winEvent.pos -= p_pos;
            return ret;
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
            PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>());
            return 0;
        }

        case WindowMessage::Resize:
        {
            auto target = winEvent.objHndl.lock();
            if (target && target != p_weakSelf.lock())
            {
                target->PostEvent(CreateWindowEvent<WindowMessage::Resize>(winEvent.objHndl, winEvent.size));
                return 0;
            }

            p_size = winEvent.size;
            PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>());

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
                p_parent.lock()->PostEvent(CreateWindowEvent<WindowMessage::Resize>(target, winEvent.size));

            return 0;
        }

        default:
            return 0;
        }
    }

    return GalvanizerObject::Callback(event);
}
