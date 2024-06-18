#include "EventConfigurations.h"

using namespace Galvanizer;


void EventConfiguration::ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Init> tag, std::shared_ptr<ObjectEvent> event)
{
    event->visibility = EventVisibility::Root;
    event->priority = ChildPriority::Last;
}


void EventConfiguration::ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Run> tag, std::shared_ptr<ObjectEvent> event)
{
    event->visibility = EventVisibility::Root;
    event->priority = ChildPriority::Last;
}

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Close> tag, std::shared_ptr<ObjectEvent> event)
{
    event->visibility = EventVisibility::Root;
    event->priority = ChildPriority::First;
}

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Terminate> tag, std::shared_ptr<ObjectEvent> event, GObjHNDL objHndl)
{
    event->visibility = EventVisibility::Single;
    event->objHndl = objHndl;
}

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Empty> tag, std::shared_ptr<ObjectEvent> event)
{
    event->visibility = EventVisibility::Single;
}

