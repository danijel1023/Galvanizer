#include <iostream>

#include "MainWindow.h"
#include "Application.h"
#include "EventConfigurations.h"

#include "GLFW/glfw3.h"

using namespace Galvanizer;
using namespace EventConfiguration;


GObjHNDL MainWindow::factory(const std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap)
{
    return new MainWindow(name, parent, originFac, createdOnHeap);
}

MainWindow::MainWindow(const std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap)
    : BaseWindow(name, parent, originFac, createdOnHeap), p_eventLoop(&m_BO, this), p_mainELRef(&p_eventLoop)
{
    // Create new ELRef
    p_eventLoopRef = &p_mainELRef;

    p_mainWindow = this;
    p_eventLoop.Start();
}

MainWindow::~MainWindow() = default;


uintptr_t MainWindow::Dispatcher(const std::shared_ptr<Event>& event)
{
    return BaseWindow::Dispatcher(event);
}


uintptr_t MainWindow::Callback(const std::shared_ptr<Event>& event)
{
    if (event->IsType<ObjectEvent>())
    {
        auto& objectEvent = static_cast<ObjectEvent&>(*event);
        switch (objectEvent.message)
        {
        case ObjectMessage::Init: {}

        default:
            break;
        }
    }

    return BaseWindow::Callback(event);
}
