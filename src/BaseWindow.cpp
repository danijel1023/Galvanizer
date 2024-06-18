#include <iostream>

#include "BaseWindow.h"
#include "MainWindow.h"
#include "Application.h"

#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;


GObjHNDL BaseWindow::factory(std::string_view name, GObjHNDL parent)
{
    return new BaseWindow(name, parent);
}


BaseWindow::BaseWindow(std::string_view name, GObjHNDL parentObj)
    : GalvanizerObject(name, parentObj)
{
    auto parent = dynamic_cast<BWinHNDL>(parentObj);

    if (parent)
        p_mainWindow = parent->p_mainWindow;
}

BaseWindow::~BaseWindow()
{}


uintptr_t BaseWindow::Dispatcher(std::shared_ptr<Event> event)
{
    return GalvanizerObject::Dispatcher(event);
}


uintptr_t BaseWindow::Callback(std::shared_ptr<Event> event)
{
    if (event->IsType<WindowEvent>())
    {
        auto windowEvent = static_cast<WindowEvent&>(*event);
        switch (windowEvent.message)
        {
        case WindowMessage::Render:
        case WindowMessage::Resize:
        case WindowMessage::Iconify:
        case WindowMessage::Maximise:
        case WindowMessage::Restore:
        {
            break;
        }

        default:
        {
            return -2;
        }
        }
    }

    return GalvanizerObject::Callback(event);
}
