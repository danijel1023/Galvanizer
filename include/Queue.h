#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>

#include "Events.h"

namespace Galvanizer
{

class IBlockingObject
{
public:
    virtual ~IBlockingObject() = default;

    virtual void WaitForEvent() = 0;
    virtual void Notify() = 0;
};


class BlockingObject : public IBlockingObject
{
public:
    void WaitForEvent() override;
    void Notify() override;

private:
    std::mutex mutex;
    std::condition_variable CV;
    bool ready = false;
};


// [TODO] Could optimise by storing object instead of Event pointer (maybe)??
class Queue
{
public:

    // Queue will NOT delete the IBlockingObject. Owner of Queue is responsible for IBO lifetime.
    explicit Queue(IBlockingObject* BO);
    ~Queue() = default;

    void WaitForEvent();

    void PostEvent(std::shared_ptr<Event> event);

    bool Empty();
    std::shared_ptr<Event> Front();
    std::shared_ptr<Event> Back();
    void Pop();

private:
    IBlockingObject* m_BO;
    std::mutex m_QAccessMutex;
    std::queue<std::shared_ptr<Event>> m_eventQueue;
};

}
