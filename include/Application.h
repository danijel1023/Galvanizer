#pragma once

#include <list>

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

    // This is a hack that I hate so much.
    OwningRef m_strongAppRef;

    GLFW_BlockingObject m_BO;
    EventLoop m_eventLoop;
    EventLoopRef m_ELRef;

    std::list<std::pair<WeakRef, void*>> m_winHNDLs;
    void* m_cursors[10] = {};

    std::thread m_timerThread;
    std::vector<std::shared_ptr<Event>> m_timerEvents;
    std::mutex m_timerEventsMutex;
    std::atomic_bool m_timerRunning = false;

private:
    uintptr_t Dispatcher(const std::shared_ptr<Event>& event) override;
    uintptr_t Callback(const std::shared_ptr<Event>& event) override;

    void TimerLoop();
};
}
