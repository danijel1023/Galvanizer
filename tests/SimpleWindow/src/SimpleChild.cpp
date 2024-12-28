#include <iostream>

#include "SimpleChild.h"
#include "EventConfigurations.h"
#include "Application.h"

using namespace Galvanizer;


GObjHNDL SimpleChild::factory(std::string_view name, const WeakRef& parent, Factory* originFac)
{
    return new SimpleChild(name, parent, originFac);
}

SimpleChild::SimpleChild(const std::string_view name, const WeakRef& parent, Factory* originFac)
    : BaseWindow(name, parent, originFac) {}


SimpleChild::~SimpleChild() = default;

uintptr_t SimpleChild::Callback(const std::shared_ptr<Event>& event)
{
    return BaseWindow::Callback(event);
}
