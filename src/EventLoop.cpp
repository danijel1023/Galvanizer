#include <iostream>
#include <unordered_map>

#include "EventLoop.h"
#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;

EventLoop::EventLoop(IBlockingObject* BO, GObjHNDL dispatchRef, bool async)
    : m_queue(BO), m_async(async), m_dispatchRef(dispatchRef) {}

EventLoop::~EventLoop()
{
    Stop();
}

void EventLoop::Start()
{
    auto empty = CreateELEvent<ELMessage::Run>();
    PostEvent(empty, nullptr);

    if (!m_async)
    {
        std::cout << "[INFO] Running loop for \"" << m_dispatchRef->GetTarget() << "\" in sync-mode: " <<
                std::this_thread::get_id() << std::endl;
        Loop();
    }
    else
    {
        m_workerThread = std::thread(&EventLoop::Loop, this);
        std::cout << "[INFO] Running loop for \"" << m_dispatchRef->GetTarget() << "\" in async-mode: " <<
                m_workerThread.get_id() << std::endl;
    }
}

void EventLoop::Stop()
{
    std::lock_guard lck(m_stopMutex);

    // This comments needs to be here bc CLion problems...
    {
        std::unique_lock lock(m_mtx);
        m_cv.wait(lock, [&] { return m_started; });
    }


    if (m_running)
    {
        m_running = false;
        PostEvent(CreateELEvent<ELMessage::Stop>(), nullptr);

        if (m_async)
        {
            std::cout << "-----[DEBUG] About to join thread-id: " << m_workerThread.get_id() << std::endl;
            m_workerThread.join();
        }
    }
    else
        std::cout << "[WARN] Calling EventLoop::Stop() on stopped thread! thread-id: " << std::this_thread::get_id() <<
                std::endl;
}


void EventLoop::PostEvent(const std::shared_ptr<Event>& event, GObjHNDL receiver)
{
    event->receiver = receiver;
    m_queue.PostEvent(event);
}

void EventLoop::Loop()
{
    m_running = true;

    // This comments needs to be here bc CLion problems...
    {
        std::lock_guard lock(m_mtx);
        m_started = true;
        m_cv.notify_all();
    }

    while (m_running)
    {
        m_queue.WaitForEvent();

        while (!m_queue.Empty())
        {
            auto event = m_queue.Front();
            m_queue.Pop();

            uintptr_t dispResult = 0;

            switch (event->visibility)
            {
            case EventVisibility::Root:
            case EventVisibility::Single:
            {
                if (event->receiver)
                    dispResult = event->receiver->Dispatcher(event);
                break;
            }

            case EventVisibility::Global:
            {
                std::cout << "[WARNING] This is not a true global event - missing definition for what \"Global\" means."
                        << std::endl;
                // [TODO - IMPORTANT] This will not send the event to the actual "Global" root! Make mechanism for
                // signaling the global root hndl

                dispResult = m_dispatchRef->Dispatcher(event);
                break;
            }

            case EventVisibility::Unset:
            {
                std::cout << "[ERROR] EventVisibility unset!" << std::endl;
                break;
            }

            default:
            {
                std::cout << "[ERROR] EventVisibility unknown!" << std::endl;
                break;
            }
            }


            if (dispResult != 0)
            {
                std::cout << "[WARN] dispResult returned non-null: " << dispResult << std::endl;
            }
        }
    }

    std::cout << "[INFO] Exiting event loop: " << std::this_thread::get_id() << std::endl;
}


EventLoopRef::EventLoopRef(EventLoop* eventLoop)
    : m_ELRef(eventLoop) {}


void EventLoopRef::set(EventLoopRef* eventLoop)
{
    m_stopMutex.lock();
    m_stopping = true;
    m_stopMutex.unlock();

    if (m_ELRef)
        m_ELRef->Stop();

    if (eventLoop)
        m_ELRef = eventLoop->m_ELRef;
    else
        m_ELRef = nullptr;

    m_stopMutex.lock();
    m_stopping = false;
    m_stopMutex.unlock();
}

void EventLoopRef::Stop()
{
    set(nullptr);
}

void EventLoopRef::PostEvent(const std::shared_ptr<Event>& event, GObjHNDL receiver)
{
    std::lock_guard stop_lck(m_stopMutex);
    if (!m_stopping)
    {
        if (m_ELRef)
            m_ELRef->PostEvent(event, receiver);
    }
}
