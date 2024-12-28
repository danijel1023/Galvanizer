#include <iostream>

#include "PluginWindowV2.h"

using namespace Galvanizer;


GObjHNDL PluginWindowV2::factory(std::string_view name, const WeakRef& parent, Factory* originFac)
{
    // Hopefully parent is not null :>
    std::cout << "PluginWindowV2 aaaaaaaaa factory as: " << parent.lock()->GetTarget() << "." << name << std::endl << std::endl;
    return new PluginWindowV2(name, parent, originFac);
}

PluginWindowV2::PluginWindowV2(const std::string_view name, const WeakRef& parent, Factory* originFac)
    : BaseWindow(name, parent, originFac) {}

PluginWindowV2::~PluginWindowV2() = default;


uintptr_t PluginWindowV2::Callback(const std::shared_ptr<Event>& event)
{
    return BaseWindow::Callback(event);
}
