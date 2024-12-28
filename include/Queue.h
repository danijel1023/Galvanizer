#pragma once

#include <queue>
#include <mutex>
#include <semaphore>

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
    std::binary_semaphore m_semaphore = std::binary_semaphore(0);
};


// [TODO] Could optimise by storing object instead of Event pointer (maybe)??
class Queue
{
public:
    // Queue will NOT delete the IBlockingObject. Owner of Queue is responsible for IBO lifetime.
    explicit Queue(IBlockingObject* BO);
    ~Queue() = default;

    void WaitForEvent();

    void PostEvent(const std::shared_ptr<Event>& event);

    bool Empty();
    const std::shared_ptr<Event>& Front();
    const std::shared_ptr<Event>& Back();
    void Pop();

private:
    IBlockingObject* m_BO;
    std::mutex m_QAccessMutex;
    std::queue<std::shared_ptr<Event> > m_eventQueue;
};
}
