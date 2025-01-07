#include <iostream>

#include "Application.h"
#include "SimpleWindow.h"

#include "SimpleChild.h"
#include "PluginWindow.h"
#include "PluginWindowV2.h"
#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;

GObjHNDL SimpleWindow::factory(const std::string_view name, const WeakRef& parent, Factory* originFac,
                               bool createdOnHeap)
{
    return new SimpleWindow(name, parent, originFac, createdOnHeap);
}

SimpleWindow::SimpleWindow(const std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap)
    : MainWindow(name, parent, originFac, createdOnHeap) {}

SimpleWindow::~SimpleWindow() = default;

uintptr_t SimpleWindow::Dispatcher(const std::shared_ptr<Event>& event)
{
    return MainWindow::Dispatcher(event);
}


uintptr_t SimpleWindow::Callback(const std::shared_ptr<Event>& event)
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


            std::cout << "====================================" << std::endl;
            std::cout << "Posting Close signal to application. thread-id: " << std::this_thread::get_id() << std::endl;

            //auto target = this;
            auto target = p_parent.lock();

            p_parent.lock()->PostEvent(CreateObjectEvent<ObjectMessage::Close>(target));
        }
    }

    return MainWindow::Callback(event);
}
