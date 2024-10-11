#include <iostream>

#include "SimpleChild.h"
#include "EventConfigurations.h"
#include "Application.h"

using namespace Galvanizer;


GObjHNDL SimpleChild::factory(std::string_view name, GObjHNDL parent, Factory* originFac)
{
    return new SimpleChild(name, parent, originFac);
}

SimpleChild::~SimpleChild()
{}

uintptr_t SimpleChild::Callback(std::shared_ptr<Event> event)
{
    if (event->IsType<ObjectEvent>())
    {
        ObjectEvent& objEvent = static_cast<ObjectEvent&>(*event);
        switch (objEvent.message)
        {
        default:
            break;
        }
    }

    return BaseWindow::Callback(event);
}

SimpleChild::SimpleChild(std::string_view name, GObjHNDL parent, Factory* originFac)
    : BaseWindow(name, parent, originFac)
{}
