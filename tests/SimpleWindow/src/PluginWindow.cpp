#include <iostream>

#include "PluginWindow.h"

using namespace Galvanizer;


GObjHNDL PluginWindow::factory(std::string_view name, GObjHNDL parent)
{
    std::cout << "PluginWindow factory as: " << parent->getPath() + '.' + std::string(name) << std::endl;
    return new PluginWindow(name, parent);
}

PluginWindow::PluginWindow(std::string_view name, GObjHNDL parent)
    : BaseWindow(name, parent)
{}

PluginWindow::~PluginWindow()
{}

uintptr_t PluginWindow::Callback(std::shared_ptr<Event> event)
{
    return BaseWindow::Callback(event);
}

