#pragma once

#include <thread>
#include <atomic>

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
    void PostEvent(const std::shared_ptr<Event>& event, GObjHNDL receiver);

private:
    Queue m_queue;
    std::thread m_workerThread;

    bool m_async = false;
    volatile std::atomic<bool> m_running;
    std::mutex m_stopMutex;

    GObjHNDL m_dispatchRef;

private:
    void Loop();
};


class EventLoopRef
{
public:
    explicit EventLoopRef(EventLoop* eventLoop);

    void swap(EventLoopRef* eventLoop);
    void Stop();
    void PostEvent(const std::shared_ptr<Event>& event, GObjHNDL receiver);

private:
    std::mutex m_refMutex;
    EventLoop* m_ELRef = nullptr;
};

}
