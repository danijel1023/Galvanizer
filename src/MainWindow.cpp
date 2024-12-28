#include <iostream>

#include "MainWindow.h"
#include "Application.h"
#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;


GObjHNDL MainWindow::factory(const std::string_view name, const WeakRef& parent, Factory* originFac)
{
    return new MainWindow(name, parent, originFac);
}

MainWindow::MainWindow(const std::string_view name, const WeakRef& parent, Factory* originFac)
    : BaseWindow(name, parent, originFac), p_eventLoop(&m_BO, this), p_mainELRef(&p_eventLoop)
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
    return BaseWindow::Callback(event);
}
