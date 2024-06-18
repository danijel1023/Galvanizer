#include "Queue.h"
#include "Events.h"


namespace Galvanizer
{

void BlockingObject::WaitForEvent()
{
    std::unique_lock<std::mutex> Lck(mutex);
    CV.wait(Lck, [&] { return ready; });

    ready = false;
}

void BlockingObject::Notify()
{
    std::lock_guard<std::mutex> lg(mutex);
    ready = true;
    CV.notify_all();
}


Queue::Queue(IBlockingObject* BO)
    : m_BO(BO)
{}


void Queue::WaitForEvent()
{
    m_BO->WaitForEvent();
}

void Queue::PostEvent(std::shared_ptr<Event> event)
{
    std::lock_guard<std::mutex> lg(m_QAccessMutex);
    m_eventQueue.push(event);
    m_BO->Notify();
}


bool Queue::Empty()
{
    std::lock_guard<std::mutex> lg(m_QAccessMutex);
    return m_eventQueue.empty();
}

std::shared_ptr<Event> Queue::Front()
{
    std::lock_guard<std::mutex> lg(m_QAccessMutex);
    return m_eventQueue.front();
}

std::shared_ptr<Event> Queue::Back()
{
    std::lock_guard<std::mutex> lg(m_QAccessMutex);
    return m_eventQueue.back();
}

void Queue::Pop()
{
    std::lock_guard<std::mutex> lg(m_QAccessMutex);
    m_eventQueue.pop();
}

}
