#pragma once

#include <mutex>
#include <string>
#include <vector>

#include "BaseWindow.h"
#include "Factory.h"
#include "Queue.h"
#include "GLFW_BlockingObject.h"
#include "EventLoop.h"

namespace Galvanizer
{

class Application
    : public GalvanizerObject
{
public:
    Application();
    ~Application() override;
    Application(Application const&&) = delete;
    static Application& get();

    int Run();

    // Target window -> path + name
    GObjHNDL FindGObj(std::string_view target);

    // Used to FindChild a specified windows (target) factory
    Factory* FindTarget(Factory* elm, std::string_view target) const;

private:
    static inline Application* m_This = nullptr;

    GLFW_BlockingObject m_BO;
    EventLoop m_eventLoop;
    EventLoopRef m_ELRef;

private:
    uintptr_t Dispatcher(std::shared_ptr<Event> event) override;
    uintptr_t Callback(std::shared_ptr<Event> event) override;
};

}
