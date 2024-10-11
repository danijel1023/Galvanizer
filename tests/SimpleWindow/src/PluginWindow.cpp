#include <iostream>

#include "PluginWindow.h"

using namespace Galvanizer;


GObjHNDL PluginWindow::factory(std::string_view name, GObjHNDL parent, Factory* originFac)
{
    std::cout << "PluginWindow factory as: " << parent->GetTarget() << std::endl;
    return new PluginWindow(name, parent, originFac);
}

PluginWindow::PluginWindow(std::string_view name, GObjHNDL parent, Factory* originFac)
    : BaseWindow(name, parent, originFac)
{}

PluginWindow::~PluginWindow()
{}

uintptr_t PluginWindow::Callback(std::shared_ptr<Event> event)
{
    return BaseWindow::Callback(event);
}

