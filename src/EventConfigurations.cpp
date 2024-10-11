#include "EventConfigurations.h"

using namespace Galvanizer;


void EventConfiguration::ConfigObjectEvent(ObjectMessageTag <ObjectMessage::Init>, std::shared_ptr<ObjectEvent> event)
{
    event->visibility = EventVisibility::Single;
}

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag <ObjectMessage::Run>, std::shared_ptr<ObjectEvent> event)
{
    event->visibility = EventVisibility::Root;
    event->priority = ChildPriority::Last;
}

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag <ObjectMessage::Close>, std::shared_ptr<ObjectEvent> event)
{
    event->visibility = EventVisibility::Single;
}

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag <ObjectMessage::Terminate>, std::shared_ptr<ObjectEvent> event, GObjHNDL objHndl)
{
    event->visibility = EventVisibility::Single;
    event->objHndl = objHndl;
}

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag <ObjectMessage::Empty>, std::shared_ptr<ObjectEvent> event)
{
    event->visibility = EventVisibility::Single;
}


void EventConfiguration::ConfigELEvent(ELMessageTag <ELMessage::Run>, std::shared_ptr<ELEvent> event)
{
    event->visibility = EventVisibility::Root;
    event->priority = ChildPriority::First;
}

void EventConfiguration::ConfigELEvent(ELMessageTag <ELMessage::Stop>, std::shared_ptr<ELEvent> event)
{
    event->visibility = EventVisibility::Root;
    event->priority = ChildPriority::Last;
}
