#include <iostream>

#include "Application.h"
#include "SimpleWindow.h"

#include "SimpleChild.h"
#include "PluginWindow.h"
#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;

Galvanizer::GObjHNDL SimpleWindow::factory(std::string_view name, GObjHNDL parent)
{
    return new SimpleWindow(name, parent);
}

SimpleWindow::SimpleWindow(std::string_view name, GObjHNDL parent)
    : Galvanizer::MainWindow(name, parent)
{}

SimpleWindow::~SimpleWindow()
{
    std::cout << "SimpleWindow destructor!" << std::endl;

    std::cout << "Posting Close signal to application." << std::endl;
    auto close = CreateObjectEvent<ObjectMessage::Close>();
    Application::get().PostEvent(close);
}


uintptr_t SimpleWindow::Callback(std::shared_ptr<Event> event)
{
    if (event->IsType<ObjectEvent>())
    {
        auto objEvent = static_cast<ObjectEvent&>(*event);

        if (objEvent.message == ObjectMessage::Init)
        {
            Factory* originalFac = Application::get().GetFactory(getTarget() + ".org");
            originalFac->ptr = &SimpleChild::factory;


            Factory* pluginFac = Application::get().GetFactory(getTarget() + ".org", "plg");
            pluginFac->ptr = &PluginWindow::factory;
            pluginFac->overridesOwner = "app";
        }

        else if (objEvent.message == ObjectMessage::Run)
        {
            auto close = CreateObjectEvent<ObjectMessage::Close>();
            PostEvent(close);
        }
    }

    return MainWindow::Callback(event);
}
