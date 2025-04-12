#include "EventConfigurations.h"

#include <utility>

using namespace Galvanizer;
using namespace EventConfiguration;

// Object

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Init>, std::shared_ptr<ObjectEvent> event)
{
    event->visibility = EventVisibility::Single;
}

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Close>, std::shared_ptr<ObjectEvent> event,
                                           std::weak_ptr<GObj> objHndl)
{
    event->visibility = EventVisibility::Single;
    event->objHndl = objHndl;
}

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Terminate>,
                                           std::shared_ptr<ObjectEvent> event, std::weak_ptr<GObj> objHndl)
{
    event->visibility = EventVisibility::Single;
    event->objHndl = objHndl;
}

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Empty>, std::shared_ptr<ObjectEvent> event)
{
    event->visibility = EventVisibility::Single;
}



// Window

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::RegisterHNDL>,
                                           std::shared_ptr<WindowEvent> event, void* winHNDL)
{
    event->visibility = EventVisibility::Single;
    event->winHNDL = winHNDL;
}


void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::CreateWindow>,
                                           std::shared_ptr<WindowEvent> event, std::weak_ptr<GObj> ret,
                                           Vec2 size, std::string_view name, void* share)
{
    event->visibility = EventVisibility::Single;
    event->size = size;
    event->name = name;
    event->share = share;
    event->objHndl = std::move(ret);
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::DestroyWindow>,
                                           std::shared_ptr<WindowEvent> event, std::weak_ptr<GObj> win)
{
    event->visibility = EventVisibility::Single;
    event->objHndl = std::move(win);
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::Maximise>,
                                           std::shared_ptr<WindowEvent> event)
{
    event->visibility = EventVisibility::Single;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::Iconify>, std::shared_ptr<WindowEvent> event)
{
    event->visibility = EventVisibility::Single;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::Restore>, std::shared_ptr<WindowEvent> event)
{
    event->visibility = EventVisibility::Single;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::FBResize>,
                                           std::shared_ptr<WindowEvent> event, std::weak_ptr<GObj> winHNDL, Vec2 size)
{
    event->visibility = EventVisibility::Single;
    event->objHndl = std::move(winHNDL);
    event->size = size;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::Resize>, std::shared_ptr<WindowEvent> event,
                                           std::weak_ptr<GObj> winHNDL, Vec2 size)
{
    event->visibility = EventVisibility::Single;
    event->objHndl = std::move(winHNDL);
    event->size = size;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::ResizeRequest>,
                                           std::shared_ptr<WindowEvent> event, Vec2 size)
{
    event->visibility = EventVisibility::Single;
    event->size = size;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::Position>,
                                           std::shared_ptr<WindowEvent> event, std::weak_ptr<GObj> winHNDL, Vec2 pos)
{
    event->visibility = EventVisibility::Single;
    event->objHndl = std::move(winHNDL);
    event->pos = pos;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::PositionRequest>,
                                           std::shared_ptr<WindowEvent> event, Vec2 pos)
{
    event->visibility = EventVisibility::Single;
    event->pos = pos;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::GainFocus>,
                                           std::shared_ptr<WindowEvent> event)
{
    event->visibility = EventVisibility::Single;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::LoseFocus>,
                                           std::shared_ptr<WindowEvent> event)
{
    event->visibility = EventVisibility::Single;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::Close>, std::shared_ptr<WindowEvent> event)
{
    event->visibility = EventVisibility::Single;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::Refresh>, std::shared_ptr<WindowEvent> event)
{
    event->visibility = EventVisibility::Single;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::Scale>, std::shared_ptr<WindowEvent> event,
                                           Vec2 scale)
{
    event->visibility = EventVisibility::Root;
    event->priority = ChildPriority::Last;
    event->scale = scale;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::RenderRequest>,
                                           std::shared_ptr<WindowEvent> event)
{
    event->visibility = EventVisibility::Single;
}



// Application
void EventConfiguration::ConfigAppEvent(AppMessageTag<AppMessage::SetCursor>, std::shared_ptr<AppEvent> event,
                                        CursorType cursorType, void* winHNDL)
{
    event->visibility = EventVisibility::Single;
    event->cursorType = cursorType;
    event->winHNDL = winHNDL;
}

void EventConfiguration::ConfigAppEvent(AppMessageTag<AppMessage::TimedEvent>, std::shared_ptr<AppEvent> event,
                                        std::shared_ptr<Event> responseEvent, std::weak_ptr<GObj> timerReceiver,
                                        std::chrono::steady_clock::time_point timeout)
{
    event->visibility = EventVisibility::Single;
    event->responseEvent = std::move(responseEvent);
    event->timeout = timeout;
    event->timerReceiver = std::move(timerReceiver);
}



// Event loop

void EventConfiguration::ConfigELEvent(ELMessageTag<ELMessage::Run>, std::shared_ptr<ELEvent> event)
{
    event->visibility = EventVisibility::Root;
    event->priority = ChildPriority::First;
}

void EventConfiguration::ConfigELEvent(ELMessageTag<ELMessage::Stop>, std::shared_ptr<ELEvent> event)
{
    event->visibility = EventVisibility::Root;
    event->priority = ChildPriority::Last;
}



// Mouse

void EventConfiguration::ConfigMouseEvent(MouseMessageTag<MouseMessage::Button>, std::shared_ptr<MouseEvent> event,
                                          Vec2 pos, MouseButton mb, MouseAction ma)
{
    event->visibility = EventVisibility::Single;
    event->pos = pos;
    event->button = mb;
    event->action = ma;
}

void EventConfiguration::ConfigMouseEvent(MouseMessageTag<MouseMessage::Enter>, std::shared_ptr<MouseEvent> event,
                                          Vec2 pos)
{
    event->visibility = EventVisibility::Single;
    event->pos = pos;
}

void EventConfiguration::ConfigMouseEvent(MouseMessageTag<MouseMessage::Leave>, std::shared_ptr<MouseEvent> event,
                                          Vec2 pos)
{
    event->visibility = EventVisibility::Single;
    event->pos = pos;
}

void EventConfiguration::ConfigMouseEvent(MouseMessageTag<MouseMessage::Move>, std::shared_ptr<MouseEvent> event,
                                          Vec2 pos)
{
    event->visibility = EventVisibility::Single;
    event->pos = pos;
}

void EventConfiguration::ConfigMouseEvent(MouseMessageTag<MouseMessage::Scroll>, std::shared_ptr<MouseEvent> event,
                                          Vec2 pos, Vec2 offset)
{
    event->visibility = EventVisibility::Single;
    event->pos = pos;
    event->scrollOffset = offset;
}



// Keyboard

void EventConfiguration::ConfigKeyEvent(KeyMessageTag<KeyMessage::Key> tag, std::shared_ptr<KeyEvent> event,
                                        KeyButton kb, int scancode, KeyAction ka)
{
    event->visibility = EventVisibility::Single;
    event->button = kb;
    event->scancode = scancode;
    event->action = ka;
}

void EventConfiguration::ConfigKeyEvent(KeyMessageTag<KeyMessage::Codepoint> tag, std::shared_ptr<KeyEvent> event,
                                        int codepoint)
{
    event->visibility = EventVisibility::Single;
    event->codepoint = codepoint;
}
