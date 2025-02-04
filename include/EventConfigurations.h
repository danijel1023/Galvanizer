#pragma once

#include "Events.h"
#include "BaseWindow.h"

// @formatter:off

namespace Galvanizer::EventConfiguration
{
template<ObjectMessage msg>
struct ObjectMessageTag {};

void ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Init> tag,       std::shared_ptr<ObjectEvent> event);
void ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Close> tag,      std::shared_ptr<ObjectEvent> event, GObjHNDL objHndl);
void ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Terminate> tag,  std::shared_ptr<ObjectEvent> event, GObjHNDL objHndl);
void ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Empty> tag,      std::shared_ptr<ObjectEvent> event);

template<ObjectMessage msg, typename... Args>
std::shared_ptr<ObjectEvent> CreateObjectEvent(Args&&... args)
{
    auto event = std::make_shared<ObjectEvent>();
    event->message = msg;
    ConfigObjectEvent(ObjectMessageTag<msg>(), event, std::forward<Args>(args)...);
    return event;
}


template<WindowMessage msg>
struct WindowMessageTag {};

void ConfigWindowEvent(WindowMessageTag<WindowMessage::RegisterHNDL>,  std::shared_ptr<WindowEvent> event, void* winHNDL);
void ConfigWindowEvent(WindowMessageTag<WindowMessage::CreateWindow>,  std::shared_ptr<WindowEvent> event, WeakRef ret, IVec2 size, std::string_view name, void* share);
void ConfigWindowEvent(WindowMessageTag<WindowMessage::DestroyWindow>, std::shared_ptr<WindowEvent> event, WeakRef winHNDL);

void ConfigWindowEvent(WindowMessageTag<WindowMessage::Maximise>,       std::shared_ptr<WindowEvent> event);
void ConfigWindowEvent(WindowMessageTag<WindowMessage::Iconify>,        std::shared_ptr<WindowEvent> event);
void ConfigWindowEvent(WindowMessageTag<WindowMessage::Restore>,        std::shared_ptr<WindowEvent> event);
void ConfigWindowEvent(WindowMessageTag<WindowMessage::Resize>,         std::shared_ptr<WindowEvent> event, IVec2 size);
void ConfigWindowEvent(WindowMessageTag<WindowMessage::Position>,       std::shared_ptr<WindowEvent> event, IVec2 pos);
void ConfigWindowEvent(WindowMessageTag<WindowMessage::GainFocus>,      std::shared_ptr<WindowEvent> event);
void ConfigWindowEvent(WindowMessageTag<WindowMessage::LoseFocus>,      std::shared_ptr<WindowEvent> event);
void ConfigWindowEvent(WindowMessageTag<WindowMessage::Close>,          std::shared_ptr<WindowEvent> event);
void ConfigWindowEvent(WindowMessageTag<WindowMessage::Refresh>,        std::shared_ptr<WindowEvent> event);
void ConfigWindowEvent(WindowMessageTag<WindowMessage::RenderRequest>,  std::shared_ptr<WindowEvent> event);

template<WindowMessage msg, typename... Args>
std::shared_ptr<WindowEvent> CreateWindowEvent(Args&&... args)
{
    auto event = std::make_shared<WindowEvent>();
    event->message = msg;
    ConfigWindowEvent(WindowMessageTag<msg>(), event, std::forward<Args>(args)...);
    return event;
}


template<AppMessage msg>
struct AppMessageTag {};

void ConfigAppEvent(AppMessageTag<AppMessage::SetCursor>,   std::shared_ptr<AppEvent> event, CursorType cursorType, void* winHNDL);
void ConfigAppEvent(AppMessageTag<AppMessage::TimedEvent>,  std::shared_ptr<AppEvent> event, std::shared_ptr<Event> responseEvent, WeakRef timerReceiver,
                                                                                             std::chrono::steady_clock::time_point timeout);

template<AppMessage msg, typename... Args>
std::shared_ptr<AppEvent> CreateAppEvent(Args&&... args)
{
    auto event = std::make_shared<AppEvent>();
    event->message = msg;
    ConfigAppEvent(AppMessageTag<msg>(), event, std::forward<Args>(args)...);
    return event;
}


template<ELMessage msg>
struct ELMessageTag {};

void ConfigELEvent(ELMessageTag<ELMessage::Stop>,   std::shared_ptr<ELEvent> event);
void ConfigELEvent(ELMessageTag<ELMessage::Run>,    std::shared_ptr<ELEvent> event);

template<ELMessage msg, typename... Args>
std::shared_ptr<ELEvent> CreateELEvent(Args&&... args)
{
    auto event = std::make_shared<ELEvent>();
    event->message = msg;
    ConfigELEvent(ELMessageTag<msg>(), event, std::forward<Args>(args)...);
    return event;
}


template<MouseMessage msg>
struct MouseMessageTag {};

void ConfigMouseEvent(MouseMessageTag<MouseMessage::Button>,    std::shared_ptr<MouseEvent> event, IVec2 pos, MouseButton mb, MouseAction ma);
void ConfigMouseEvent(MouseMessageTag<MouseMessage::Enter>,     std::shared_ptr<MouseEvent> event, IVec2 pos);
void ConfigMouseEvent(MouseMessageTag<MouseMessage::Leave>,     std::shared_ptr<MouseEvent> event, IVec2 pos);
void ConfigMouseEvent(MouseMessageTag<MouseMessage::Move>,      std::shared_ptr<MouseEvent> event, IVec2 pos);
void ConfigMouseEvent(MouseMessageTag<MouseMessage::Scroll>,    std::shared_ptr<MouseEvent> event, IVec2 pos, Vec2 offset);

template<MouseMessage msg, typename... Args>
std::shared_ptr<MouseEvent> CreateMouseEvent(Args&&... args)
{
    auto event = std::make_shared<MouseEvent>();
    event->message = msg;
    ConfigMouseEvent(MouseMessageTag<msg>(), event, std::forward<Args>(args)...);
    return event;
}


template<KeyMessage msg>
struct KeyMessageTag {};

void ConfigKeyEvent(KeyMessageTag<KeyMessage::Key> tag,         std::shared_ptr<KeyEvent> event, KeyButton kb, int scancode, KeyAction ka);
void ConfigKeyEvent(KeyMessageTag<KeyMessage::Codepoint> tag,   std::shared_ptr<KeyEvent> event, int codepoint);

template<KeyMessage msg, typename... Args>
std::shared_ptr<KeyEvent> CreateKeyEvent(Args&&... args)
{
    auto event = std::make_shared<KeyEvent>();
    event->message = msg;
    ConfigKeyEvent(KeyMessageTag<msg>(), event, std::forward<Args>(args)...);
    return event;
}
}
