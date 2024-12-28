#include <iostream>

#include "Application.h"
#include "GalvanizerObject.h"
#include "EventConfigurations.h"
#include "Factory.h"

using namespace Galvanizer;
using namespace EventConfiguration;


GObjHNDL GalvanizerObject::factory(const std::string_view name, const WeakRef& parent, Factory* originFac)
{
    return new GalvanizerObject(name, parent, originFac);
}

GalvanizerObject::GalvanizerObject(const std::string_view name, const WeakRef& parent, Factory* originFac)
    : p_parent(parent), pc_internalName(name), p_displayName(name), m_originFac(originFac)
{
    std::cout << "[DEBUG] " << pc_internalName << " created on thread-id " << std::this_thread::get_id() << std::endl;

    const OwningRef lockedParent = parent.lock();
    if (lockedParent.get())
        p_eventLoopRef = lockedParent.get()->p_eventLoopRef;
}


GalvanizerObject::~GalvanizerObject()
{
    if (m_originFac)
        m_originFac->built = false;

    std::cout << "Bailing out: " << p_displayName << std::endl;
}


uintptr_t GalvanizerObject::Dispatcher(const std::shared_ptr<Event>& event)
{
    switch (event->visibility)
    {
    case EventVisibility::Single:
        return Callback(event);

    case EventVisibility::Global:
    case EventVisibility::Root:
    {
        if (event->priority == ChildPriority::Last)
            Callback(event);

        for (const auto& ch: p_children)
        {
            // If the child is on another thread, add the event to its queue and continue
            if (*ch->p_eventLoopRef != *p_eventLoopRef)
                ch->PostEvent(event);
            else
                ch->Dispatcher(event);
        }

        if (event->priority == ChildPriority::First)
            Callback(event);

        return 0;
    }

    default:
        return -1;
    }
}


uintptr_t GalvanizerObject::Callback(const std::shared_ptr<Event>& event)
{
    std::cout << "[DEBUG::" << GetTarget()
            << " in GalvanizerObject::Callback] Event: " << event->strMessage() << " thread-id: " <<
            std::this_thread::get_id() << std::endl;

    if (event->IsType<ObjectEvent>())
    {
        auto& objectEvent = static_cast<ObjectEvent&>(*event);
        switch (objectEvent.message)
        {
        case ObjectMessage::Init:
        {
            if (!p_weakSelf.lock().get())
            {
                std::cout << "[ERROR] p_weakSelf is null on " << p_displayName << ": " << this
                        << "; imminent crash!" << std::endl;
            }

            auto children = ObjectFactories::GetInstance().Build(p_weakSelf.lock());
            for (const auto& ch: children)
            {
                p_children.push_back(ch);

                // Post init - if ch is on a different thread, it'll use the new thread
                if (*ch->p_eventLoopRef != *p_eventLoopRef)
                    ch->PostEvent(EventConfiguration::CreateObjectEvent<ObjectMessage::Init>());
                else
                    ch->Dispatcher(EventConfiguration::CreateObjectEvent<ObjectMessage::Init>());
            }

            break;
        }

        case ObjectMessage::Close:
        {
            GObjHNDL target = objectEvent.objHndl;

            // The parent
            if (target->p_parent.lock() == this)
            {
                if (*target->p_eventLoopRef != *p_eventLoopRef)
                {
                    target->PostEvent(event);

                    std::cout << "[DEBUG] Shutting down target thread (" << target->p_displayName << ")... thread-id: "
                            << std::this_thread::get_id() << std::endl;
                    target->p_eventLoopRef->set(p_eventLoopRef);
                    std::cout << "-- Done" << std::endl;
                }

                else
                    target->Dispatcher(event);

                if (const auto parent = p_parent.lock())
                    std::cout << "Posting Term to " << parent->p_displayName << std::endl;
                else
                    std::cout << "[no parent] Posting Term to " << p_displayName << std::endl;

                PostEvent(EventConfiguration::CreateObjectEvent<ObjectMessage::Terminate>(target));
            }

            // Targeted child
            else
            {
                m_closing = true;
                for (const auto& ch: p_children)
                {
                    if (*ch->p_eventLoopRef != *p_eventLoopRef)
                    {
                        ch->PostEvent(event);

                        std::cout << "[DEBUG] Shutting down child thread (" << ch->p_displayName << ")... thread-id: "
                                << std::this_thread::get_id() << std::endl;

                        // [TODO] Sometimes, this calls stop on stopped thread??
                        ch->p_eventLoopRef->set(p_eventLoopRef);

                        std::cout << "[DEBUG] Shutting down child thread (" << ch->p_displayName << ")... thread-id: "
                                << std::this_thread::get_id() << " -- Done -------" << std::endl;
                    }

                    else
                        ch->Dispatcher(event);
                }
            }

            if (!target->p_parent.lock())
                PostEvent(EventConfiguration::CreateObjectEvent<ObjectMessage::Terminate>(target));

            /*if (this != &Application::get())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }*/

            break;
        }

        case ObjectMessage::Terminate:
        {
            GObjHNDL target = objectEvent.objHndl;

            // The parent
            if (target->p_parent.lock() == this)
            {
                std::cout << "[DEBUG] Parent terminate - Recursive call..." << std::endl;
                target->Dispatcher(event);

                std::cout << "[DEBUG] Parent terminate - Delete child..." << std::endl;
                for (size_t i = 0; i < p_children.size(); i++)
                {
                    if (p_children[i] != target)
                        continue;

                    p_children.erase(p_children.begin() + i);
                    break;
                }

                std::cout << "[DEBUG] Parent terminate - Done" << std::endl;
            }

            // Target and its children
            else
            {
                for (auto& ch: p_children)
                {
                    ch->Dispatcher(event);

                    std::cout << "Killing \"" << ch->GetTarget() << "\" thread-id: " <<
                            std::this_thread::get_id() << std::endl;

                    ch.DropOwnership();
                }

                p_children.clear();
            }

            break;
        }

        case ObjectMessage::Empty:
            break;
        }
    }

    else if (event->IsType<ELEvent>())
    {
        auto elEvent = static_cast<ELEvent&>(*event);
        const auto lockedParent = p_parent.lock();

        // If current thread is not the same as the thread of the parent object, stop the current thread
        if (elEvent.message == ELMessage::Stop && lockedParent && *p_eventLoopRef != *lockedParent->p_eventLoopRef)
            p_eventLoopRef->set(lockedParent->p_eventLoopRef);
    }

    return 0;
}


WeakRef GalvanizerObject::FindChild(std::string_view name)
{
    for (const auto& win: p_children)
    {
        if (win->pc_internalName == name)
            return win;
    }

    return {};
}


std::string GalvanizerObject::GetPath() const
{
    if (const auto lockedParent = p_parent.lock(); !lockedParent)
        return "";
    else
        return lockedParent->GetTarget();
}

std::string GalvanizerObject::GetTarget() const
{
    if (const auto lockedParent = p_parent.lock(); !lockedParent)
        return pc_internalName;
    else
        return lockedParent->GetTarget() + "." + pc_internalName;
}


void GalvanizerObject::PostEvent(const std::shared_ptr<Event>& event)
{
    p_eventLoopRef->PostEvent(event, this);
}
