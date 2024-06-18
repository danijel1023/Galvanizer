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

class Application : public GalvanizerObject
{
public:
    static Application& get();
    Application(Application const&&) = delete;

    int Run();

    // Target window -> path + name
    GObjHNDL FindGObj(std::string_view target);

    // Target factory -> *ownerName* + path + name
    Factory* FindFactory(std::string_view target, std::string_view owner);

    // Get will create a factory if the factory doesn't exist. If a factory exists,
    // it will instead reset all values to default. Target includes the ownerName name
    Factory* GetFactory(std::string_view target, std::string_view owner = "app");

    // Build full namespace
    std::vector<GObjHNDL> Build(std::string_view path);

    // Deconstruct single target
    bool Deconstruct(GObjHNDL obj);

    // Used to Find a specified windows (target) factory
    Factory* FindTarget(Factory* elm, std::string_view target) const;


private:
    std::vector<FactoryRoot*> m_factoryRoots;
    std::recursive_mutex m_factoryMutex;

    GLFW_BlockingObject m_BO;
    EventLoop m_eventLoop;

    bool m_closing = false;

private:
    Application();

    uintptr_t Dispatcher(std::shared_ptr<Event> event) override;
    uintptr_t Callback(std::shared_ptr<Event> event) override;
};

}
