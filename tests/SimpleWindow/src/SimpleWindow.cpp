#include <iostream>

#include "Application.h"
#include "SimpleWindow.h"

#include "SimpleChild.h"
#include "PluginWindow.h"
#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;

Galvanizer::GObjHNDL SimpleWindow::factory(std::string_view name, GObjHNDL parent, Factory* originFac)
{
    return new SimpleWindow(name, parent, originFac);
}

SimpleWindow::SimpleWindow(std::string_view name, GObjHNDL parent, Factory* originFac)
    : Galvanizer::MainWindow(name, parent, originFac)
{}

SimpleWindow::~SimpleWindow()
{}


uintptr_t SimpleWindow::Callback(std::shared_ptr<Event> event)
{
    if (event->IsType<ObjectEvent>())
    {
        auto objEvent = static_cast<ObjectEvent&>(*event);

        if (objEvent.message == ObjectMessage::Init)
        {
            Factory* originalFac = ObjectFactories::GetInstance().Get(GetTarget() + ".org");
            originalFac->ptr = &SimpleChild::factory;


            ObjectFactories::GetInstance().CreateOwner("plg");
            Factory* pluginFac = ObjectFactories::GetInstance().Get(GetTarget() + ".org", "plg");
            pluginFac->ptr = &PluginWindow::factory;
            pluginFac->overridesOwner = "app";
        }

        else if (objEvent.message == ObjectMessage::Run)
        {
            std::cout << "====================================" << std::endl;
            std::cout << "Posting Close signal to application." << std::endl;

            auto close = CreateObjectEvent<ObjectMessage::Close>();
            Application::get().PostEvent(close);
        }
    }

    return MainWindow::Callback(event);
}
