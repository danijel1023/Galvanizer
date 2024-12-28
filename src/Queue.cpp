#include "Queue.h"
#include "Events.h"


namespace Galvanizer
{
void BlockingObject::WaitForEvent()
{
    m_semaphore.acquire();
}

void BlockingObject::Notify()
{
    m_semaphore.release();
}


Queue::Queue(IBlockingObject* BO)
    : m_BO(BO) {}


void Queue::WaitForEvent()
{
    m_BO->WaitForEvent();
}

void Queue::PostEvent(const std::shared_ptr<Event>& event)
{
    std::lock_guard lg(m_QAccessMutex);
    m_eventQueue.push(event);
    m_BO->Notify();
}


bool Queue::Empty()
{
    std::lock_guard lg(m_QAccessMutex);
    return m_eventQueue.empty();
}

const std::shared_ptr<Event>& Queue::Front()
{
    std::lock_guard lg(m_QAccessMutex);
    return m_eventQueue.front();
}

const std::shared_ptr<Event>& Queue::Back()
{
    std::lock_guard lg(m_QAccessMutex);
    return m_eventQueue.back();
}

void Queue::Pop()
{
    std::lock_guard lg(m_QAccessMutex);
    m_eventQueue.pop();
}
}
