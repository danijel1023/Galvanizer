#pragma once

#include "BaseWindow.h"
#include "GLFW_BlockingObject.h"
#include "EventLoop.h"

namespace Galvanizer
{
class Application : public GalvanizerObject
{
public:
    Application();
    ~Application() override;
    Application(Application const&&) = delete;
    static Application& get();

    int Run();

    // Target window -> path + name
    OwningRef FindGObj(std::string_view target);

    // Used to FindChild a specified windows (target) factory
    Factory* FindTarget(Factory* elm, std::string_view target) const;

private:
    static inline GObjHNDL m_ApplicationThis;

    // This is a hack that I hate so fk much.
    OwningRef m_strongAppRef;

    GLFW_BlockingObject m_BO;
    EventLoop m_eventLoop;
    EventLoopRef m_ELRef;

private:
    uintptr_t Dispatcher(const std::shared_ptr<Event>& event) override;
    uintptr_t Callback(const std::shared_ptr<Event>& event) override;
};
}
