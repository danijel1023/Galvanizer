#pragma once

#include <list>

#include "BaseWindow.h"
#include "GLFW_BlockingObject.h"
#include "EventLoop.h"

namespace Galvanizer
{
enum class Platform
{
    Null, X11, Wayland, Windows, MacOS
};

class Application : public GalvanizerObject
{
public:
    ~Application() override;

    static void Init();
    static std::shared_ptr<Application> get();
    static Platform GetPlatform() { return get()->m_platform; }

    int Run();

    // Target window -> path + name
    std::shared_ptr<GObj> FindGObj(std::string_view target);

protected:
    Application();

private:
    static inline std::shared_ptr<Application> m_ApplicationThis;
    Platform m_platform;

    GLFW_BlockingObject m_BO;
    EventLoop m_eventLoop;
    EventLoopRef m_ELRef;

    std::list<std::pair<std::weak_ptr<GObj>, void*>> m_winHNDLs;
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
