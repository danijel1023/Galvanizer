#include <iostream>

#include "SimpleChild.h"
#include "MainWindow.h"
#include "EventConfigurations.h"

using namespace Galvanizer;


GObjHNDL SimpleChild::factory(std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap)
{
    return new SimpleChild(name, parent, originFac, createdOnHeap);
}

SimpleChild::SimpleChild(const std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap)
    : BaseWindow(name, parent, originFac, createdOnHeap) {}


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
            p_pos = {100, 100};
            p_size = {100, 200};

            m_bkg.color = {200.0 / 255, 200.0 / 255, 200.0 / 255, 1.0};
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
            p_mainWindow->renderer.SetSpace(winEvent.pos + p_pos, p_size);
            p_mainWindow->renderer.AddQuad(m_bkg);
            p_mainWindow->renderer.Render();
            break;
        }

        default:
            break;
        }
    }

    return BaseWindow::Callback(event);
}
