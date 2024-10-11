#include <iostream>

#include "MainWindow.h"
#include "Application.h"
#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;


GObjHNDL MainWindow::factory(std::string_view name, GObjHNDL parent, Factory* originFac)
{
    return new MainWindow(name, parent, originFac);
}

MainWindow::MainWindow(std::string_view name, GObjHNDL parent, Factory* originFac)
    : BaseWindow(name, parent, originFac), p_eventLoop(&m_BO, this), p_mainELRef(&p_eventLoop)
{
    // Create new ELRef
    p_eventLoopRef = &p_mainELRef;

    p_mainWindow = this;
    p_eventLoop.Start();
}

MainWindow::~MainWindow()
{}


uintptr_t MainWindow::Dispatcher(std::shared_ptr<Event> event)
{
    return BaseWindow::Dispatcher(event);
}


uintptr_t MainWindow::Callback(std::shared_ptr<Event> event)
{
    if (event->IsType<ObjectEvent>())
    {
        auto objEvent = static_cast<ObjectEvent&>(*event);
        switch (objEvent.message)
        {
        case ObjectMessage::Run:
        {
            break;
        }

        default:
            break;
        }
    }

    return BaseWindow::Callback(event);
}
