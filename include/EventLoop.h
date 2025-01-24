#pragma once

#include <thread>
#include <atomic>
#include <condition_variable>

#include "Queue.h"


namespace Galvanizer
{
class GalvanizerObject;
using GObjHNDL = GalvanizerObject*;

class EventLoop
{
public:
    EventLoop(IBlockingObject* BO, GObjHNDL dispatchRef, bool async = true);
    ~EventLoop();

    // If async is set to false, Start() function WILL BLOCK the calling thread
    void Start();
    void Stop();

    // Thread safe
    void PostEvent(const std::shared_ptr<Event>& event, const WeakRef& receiver);
    bool Running() { return m_running; }

private:
    Queue m_queue;
    std::thread m_workerThread;

    bool m_async = false;
    volatile std::atomic<bool> m_running;
    std::mutex m_stopMutex;

    GObjHNDL m_dispatchRef;

    std::condition_variable m_cv;
    std::mutex m_startMutex;
    bool m_started = false;

    void Loop();
};


class EventLoopRef
{
public:
    explicit EventLoopRef(EventLoop* eventLoop);

    void set(EventLoopRef* eventLoop);
    void Stop();

    // Will drop events while switching event loop reference
    void PostEvent(const std::shared_ptr<Event>& event, WeakRef receiver);

    // [WARN] Not thread safe!
    bool operator==(const EventLoopRef& right) const
    {
        return m_ELRef == right.m_ELRef;
    }

private:
    EventLoop* m_ELRef = nullptr;

    std::mutex m_stopMutex;
    bool m_stopping = false;
};
}
