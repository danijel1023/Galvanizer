#include <iostream>

#include "EventLoop.h"
#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;


EventLoop::EventLoop(IBlockingObject* BO, GObjHNDL dispatchRef, bool async)
    : m_queue(BO), m_async(async), m_dispatchRef(dispatchRef)
{}

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
        std::cout << "[INFO] Running loop in sync-mode: " << std::this_thread::get_id() << std::endl;
        Loop();
    }
    else
    {
        m_workerThread = std::thread(&EventLoop::Loop, this);
        std::cout << "[INFO] Started thread: " << m_workerThread.get_id() << std::endl;
    }
}

void EventLoop::Stop()
{
    std::lock_guard lck(m_stopMutex);

    if (m_running)
    {
        m_running = false;
        auto empty = CreateELEvent<ELMessage::Stop>();
        PostEvent(empty, nullptr);

        if (m_async)
            m_workerThread.join();
    }
}


void EventLoop::PostEvent(const std::shared_ptr<Event>& event, GObjHNDL receiver)
{
    event->receiver = receiver;
    m_queue.PostEvent(event);
}

void EventLoop::Loop()
{
    m_running = true;

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
    : m_ELRef(eventLoop)
{}


void EventLoopRef::swap(EventLoopRef* eventLoop)
{
    std::lock_guard lck(m_refMutex);
    if (m_ELRef)
        m_ELRef->Stop();

    if (eventLoop)
        m_ELRef = eventLoop->m_ELRef;
    else
        m_ELRef = nullptr;
}

void EventLoopRef::Stop()
{
    swap(nullptr);
}

void EventLoopRef::PostEvent(const std::shared_ptr<Event>& event, GObjHNDL receiver)
{
    std::lock_guard lck(m_refMutex);

    if (m_ELRef)
        m_ELRef->PostEvent(event, receiver);
}
