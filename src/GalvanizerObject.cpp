#include <iostream>

#include "Application.h"
#include "GalvanizerObject.h"
#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;


GObjHNDL GalvanizerObject::factory(std::string_view name, GObjHNDL parent,
                                   Factory* originFac)
{
    return new GalvanizerObject(name, parent, originFac);
}

GalvanizerObject::GalvanizerObject(std::string_view name, GObjHNDL parent, Factory* originFac)
    : p_parent(parent), pc_internalName(name), p_displayName(name), m_originFac(originFac)
{
    std::cout << "[DEBUG] " << pc_internalName << " created on " << std::this_thread::get_id() << std::endl;

    if (parent)
    {
        parent->p_children.push_back(this);
        p_eventLoopRef = parent->p_eventLoopRef;
    }
}

GalvanizerObject::~GalvanizerObject()
{
    for (auto ch: p_children)
        delete ch;

    if (m_originFac)
        m_originFac->built = false;
}


uintptr_t GalvanizerObject::Dispatcher(std::shared_ptr<Event> event)
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

        for (auto ch: p_children)
        {
            // If the child is on another thread, add the event to its queue and continue
            if (ch->p_eventLoopRef != p_eventLoopRef)
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


uintptr_t GalvanizerObject::Callback(std::shared_ptr<Event> event)
{
    std::cout << "[DEBUG::" << GetTarget()
              << " in GalvanizerObject::Callback] Event: " << event->strMessage() << std::endl;

    if (event->IsType<ObjectEvent>())
    {
        auto& objectEvent = static_cast<ObjectEvent&>(*event);
        switch (objectEvent.message)
        {
        case ObjectMessage::Init:
        {
            break;
        }

        case ObjectMessage::Close:
        {
            auto terminate = CreateObjectEvent<ObjectMessage::Terminate>(this);
            if (p_parent)
                p_parent->PostEvent(terminate);
            else
                PostEvent(terminate);

            break;
        }

        case ObjectMessage::Terminate:
        {
            for (size_t i = 0; i < p_children.size(); i++)
            {
                if (p_children[i] != objectEvent.objHndl)
                    continue;

                p_children.erase(p_children.begin() + i);
                break;
            }

            delete objectEvent.objHndl;
            break;
        }

        case ObjectMessage::Run:
        {
            ObjectFactories::GetInstance().Build(this);
            //Application::get().Build(GetTarget());
            break;
        }

        case ObjectMessage::Empty:
            break;
        }
    }

    else if (event->IsType<ELEvent>())
    {
        auto elEvent = static_cast<ELEvent&>(*event);

        // If current thread is not the same as the thread of the parent object, stop the current thread
        if (elEvent.message == ELMessage::Stop && p_parent && p_eventLoopRef != p_parent->p_eventLoopRef)
            p_eventLoopRef->swap(p_parent->p_eventLoopRef);
    }

    return 0;
}


GObjHNDL GalvanizerObject::FindChild(std::string_view name)
{
    for (const auto& win: p_children)
    {
        if (win->pc_internalName == name)
            return win;
    }

    return nullptr;
}


std::string GalvanizerObject::GetPath() const
{
    if (!p_parent)
        return "";
    else
        return p_parent->GetTarget();
}

std::string GalvanizerObject::GetTarget() const
{
    if (!p_parent)
        return pc_internalName;
    else
        return p_parent->GetTarget() + "." + pc_internalName;
}


void GalvanizerObject::PostEvent(std::shared_ptr<Event> event)
{
    p_eventLoopRef->PostEvent(event, this);
}
