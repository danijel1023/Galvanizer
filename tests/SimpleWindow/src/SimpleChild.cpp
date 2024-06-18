#include <iostream>

#include "SimpleChild.h"

using namespace Galvanizer;


GObjHNDL SimpleChild::factory(std::string_view name, GObjHNDL parent)
{
    return new SimpleChild(name, parent);
}

SimpleChild::~SimpleChild()
{
    std::cout << "SimpleChild destructor!" << std::endl;
}

uintptr_t SimpleChild::Callback(std::shared_ptr<Event> event)
{
    return BaseWindow::Callback(event);
}

SimpleChild::SimpleChild(std::string_view name, GObjHNDL parent)
    : BaseWindow(name, parent)
{
    std::cout << "SimpleChild constructor!" << std::endl;
}
