#include <iostream>

#include "PluginWindowV2.h"

using namespace Galvanizer;

namespace
{
struct PluginWindowV2_Shared : PluginWindowV2
{
    template<typename... Args>
    PluginWindowV2_Shared(Args&&... args): PluginWindowV2(std::forward<Args>(args)...) {}
};
}

std::shared_ptr<GObj> PluginWindowV2::factory(std::string_view name, const std::weak_ptr<GObj>& parent,
                                              Factory* originFac)
{
    // Hopefully parent is not null :>
    std::cout << "PluginWindowV2 aaaaaaaaa factory as: " << parent.lock()->GetTarget() << "." << name << std::endl <<
            std::endl;

    return std::make_shared<PluginWindowV2_Shared>(name, parent, originFac);
}

PluginWindowV2::PluginWindowV2(const std::string_view name, const std::weak_ptr<GObj>& parent, Factory* originFac)
    : BaseWindow(name, parent, originFac) {}

PluginWindowV2::~PluginWindowV2() = default;


uintptr_t PluginWindowV2::Callback(const std::shared_ptr<Event>& event)
{
    return BaseWindow::Callback(event);
}
