#pragma once

#include "Events.h"
#include "BaseWindow.h"


namespace Galvanizer::EventConfiguration
{


template<WindowMessage msg>
struct WindowMessageTag
{};

//void ConfigWindowEvent(WindowMessageTag<WindowMessage::Close>, std::shared_ptr<WindowEvent> event, BWinHNDL win);

template<WindowMessage msg, typename... Args>
std::shared_ptr<WindowEvent> CreateWindowEvent(Args&& ... args)
{
    auto event = std::make_shared<WindowEvent>();
    ConfigWindowEvent(WindowMessageTag<msg>(), event, std::forward<Args>(args)...);
    return event;
}


template<ObjectMessage msg>
struct ObjectMessageTag
{};

void ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Init> tag, std::shared_ptr<ObjectEvent> event);
void ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Run> tag, std::shared_ptr<ObjectEvent> event);
void ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Close> tag, std::shared_ptr<ObjectEvent> event);
void ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Terminate> tag, std::shared_ptr<ObjectEvent> event, GObjHNDL objHndl);
void ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Empty> tag, std::shared_ptr<ObjectEvent> event);

template<ObjectMessage msg, typename... Args>
std::shared_ptr<ObjectEvent> CreateObjectEvent(Args&& ... args)
{
    auto event = std::make_shared<ObjectEvent>();
    event->message = msg;
    ConfigObjectEvent(ObjectMessageTag<msg>(), event, std::forward<Args>(args)...);
    return event;
}


template<AppMessage msg>
struct AppMessageTag
{};

void ConfigAppEvent(AppMessageTag<AppMessage::Nothing>, std::shared_ptr<AppEvent> event);

template<AppMessage msg, typename... Args>
std::shared_ptr<AppEvent> CreateAppEvent(Args&& ... args)
{
    auto event = std::make_shared<AppEvent>();
    event->message = msg;
    ConfigAppEvent(AppMessageTag<msg>(), event, std::forward<Args>(args)...);
    return event;
}
}
