#include <iostream>

#include "Application.h"
#include "GalvanizerObject.h"
#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;


GObjHNDL GalvanizerObject::factory(std::string_view name, GObjHNDL parent)
{
    return new GalvanizerObject(name, parent);
}

GalvanizerObject::GalvanizerObject(std::string_view name, GObjHNDL parent)
    : p_parent(parent), p_name(name)
{
    if (parent)
        p_eventLoopRef = parent->p_eventLoopRef;
}

GalvanizerObject::~GalvanizerObject()
{
    for (auto ch: p_children)
        delete ch;
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
            if (ch->p_eventLoopRef == p_eventLoopRef)
                ch->Dispatcher(event);
            else
                ch->PostEvent(event);
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
    std::cout << "[DEBUG::" << getTarget()
              << " in GalvanizerObject::Callback] Event: " << event->strMessage() << std::endl;

    if (event->IsType<ObjectEvent>())
    {
        auto& objectEvent = static_cast<ObjectEvent&>(*event);
        switch (objectEvent.message)
        {
        case ObjectMessage::Init:
        {
            Application::get().Build(getTarget());
            break;
        }

        case ObjectMessage::Close:
        {
            if (p_parent)
            {
                auto terminate = CreateObjectEvent<ObjectMessage::Terminate>(this);
                p_parent->PostEvent(terminate);
            }

            break;
        }

        case ObjectMessage::Terminate:
        {
            Application::get().Deconstruct(objectEvent.objHndl);
            break;
        }

        case ObjectMessage::Empty:
        case ObjectMessage::Run:
            break;
        }
    }
    return 0;
}


GObjHNDL GalvanizerObject::FindChild(std::string_view name)
{
    for (const auto& win: p_children)
    {
        if (win->p_name == name)
            return win;
    }

    return nullptr;
}


std::string GalvanizerObject::getPath() const
{
    if (p_parent)
    {
        if (!p_parent->p_parent)
            return "";
        else
            return p_parent->getTarget();
    }
    else
        return "";
}

std::string GalvanizerObject::getTarget() const
{
    if (p_parent)
    {
        if (!p_parent->p_parent)
            return p_name;
        else
            return p_parent->getTarget() + "." + p_name;
    }
    else
        return "";
}


void GalvanizerObject::PostEvent(std::shared_ptr<Event> event)
{
    if (DEBUG_LOG_LVL >= 1 && !p_eventLoopRef)
    {
        std::cout << "[ERROR]: Object (" << this << "): p_eventLoopRef is null!" << std::endl;
        return;
    }

    p_eventLoopRef->PostEvent(event, this);
}
