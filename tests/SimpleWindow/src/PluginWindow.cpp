#include <iostream>

#include "PluginWindow.h"

using namespace Galvanizer;


GObjHNDL PluginWindow::factory(std::string_view name, const WeakRef& parent, Factory* originFac)
{
    // Hopefully parent is not null :>
    std::cout << "PluginWindow factory as: " << parent.lock()->GetTarget() << "." << name << std::endl << std::endl;
    return new PluginWindow(name, parent, originFac);
}

PluginWindow::PluginWindow(const std::string_view name, const WeakRef& parent, Factory* originFac)
    : MainWindow(name, parent, originFac) {}

PluginWindow::~PluginWindow() = default;


uintptr_t PluginWindow::Callback(const std::shared_ptr<Event>& event)
{
    return MainWindow::Callback(event);
}
