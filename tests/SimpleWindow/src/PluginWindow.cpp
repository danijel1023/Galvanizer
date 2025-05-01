#include <iostream>

#include "PluginWindow.h"

using namespace Galvanizer;

namespace
{
struct PluginWindow_Shared : PluginWindow
{
    template<typename... Args>
    PluginWindow_Shared(Args&&... args): PluginWindow(std::forward<Args>(args)...) {}
};
}

std::shared_ptr<GObj> PluginWindow::factory(std::string_view name, const std::weak_ptr<GObj>& parent,
                                            Factory* originFac)
{
    // Hopefully parent is not null :>
    std::cout << "PluginWindow factory as: " << parent.lock()->GetTarget() << "." << name << std::endl << std::endl;
    return std::make_shared<PluginWindow_Shared>(name, parent, originFac);
}

PluginWindow::PluginWindow(const std::string_view name, const std::weak_ptr<GObj>& parent, Factory* originFac)
    : MainWindow(name, parent, originFac) {}

PluginWindow::~PluginWindow() = default;


uintptr_t PluginWindow::Callback(const std::shared_ptr<Event>& event)
{
    if (event->IsType<ObjectEvent>())
    {
        auto& objEvent = static_cast<ObjectEvent&>(*event);

        switch (objEvent.message)
        {
        case ObjectMessage::Init:
        {
            //p_size = {854, 480};
            SetVirtualSize({300, 300});

            break;
        }

        case ObjectMessage::Terminate:
        {
            break;
        }

        default:
            break;
        }
    }

    return MainWindow::Callback(event);
}
