#include <iostream>

#include "BaseWindow.h"
#include "MainWindow.h"
#include "Application.h"

#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;


GObjHNDL BaseWindow::factory(std::string_view name, const WeakRef& parent, Factory* originFac)
{
    return new BaseWindow(name, parent, originFac);
}


BaseWindow::BaseWindow(const std::string_view name, const WeakRef& parent, Factory* originFac)
    : GalvanizerObject(name, parent, originFac)
{
    const auto lockedParent = parent.lock();

    if (const auto winParent = dynamic_cast<BWinHNDL>(lockedParent.get()))
        p_mainWindow = winParent->p_mainWindow;
}

BaseWindow::~BaseWindow() = default;


uintptr_t BaseWindow::Dispatcher(const std::shared_ptr<Event>& event)
{
    return GalvanizerObject::Dispatcher(event);
}


uintptr_t BaseWindow::Callback(const std::shared_ptr<Event>& event)
{
    return GalvanizerObject::Callback(event);
}
