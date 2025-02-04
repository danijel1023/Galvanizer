#include "EventConfigurations.h"

using namespace Galvanizer;
using namespace EventConfiguration;

// Object

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Init>, std::shared_ptr<ObjectEvent> event)
{
    event->visibility = EventVisibility::Single;
}

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Close>, std::shared_ptr<ObjectEvent> event,
                                           GObjHNDL objHndl)
{
    event->visibility = EventVisibility::Single;
    event->objHndl = objHndl;
}

void EventConfiguration::ConfigObjectEvent(ObjectMessageTag<ObjectMessage::Terminate>,
                                           std::shared_ptr<ObjectEvent> event, GObjHNDL objHndl)
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
                                           std::shared_ptr<WindowEvent> event, WeakRef ret,
                                           IVec2 size, std::string_view name, void* share)
{
    event->visibility = EventVisibility::Single;
    event->size = size;
    event->name = name;
    event->share = share;
    event->objHndl = ret;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::DestroyWindow>,
                                           std::shared_ptr<WindowEvent> event, WeakRef win)
{
    event->visibility = EventVisibility::Single;
    event->objHndl = win;
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

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::Resize>, std::shared_ptr<WindowEvent> event,
                                           IVec2 size)
{
    event->visibility = EventVisibility::Single;
    event->size = size;
}

void EventConfiguration::ConfigWindowEvent(WindowMessageTag<WindowMessage::Position>,
                                           std::shared_ptr<WindowEvent> event, IVec2 pos)
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
                                        std::shared_ptr<Event> responseEvent, WeakRef timerReceiver,
                                        std::chrono::steady_clock::time_point timeout)
{
    event->visibility = EventVisibility::Single;
    event->responseEvent = responseEvent;
    event->timeout = timeout;
    event->timerReceiver = timerReceiver;
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
                                          IVec2 pos, MouseButton mb, MouseAction ma)
{
    event->visibility = EventVisibility::Single;
    event->pos = pos;
    event->button = mb;
    event->action = ma;
}

void EventConfiguration::ConfigMouseEvent(MouseMessageTag<MouseMessage::Enter>, std::shared_ptr<MouseEvent> event,
                                          IVec2 pos)
{
    event->visibility = EventVisibility::Single;
    event->pos = pos;
}

void EventConfiguration::ConfigMouseEvent(MouseMessageTag<MouseMessage::Leave>, std::shared_ptr<MouseEvent> event,
                                          IVec2 pos)
{
    event->visibility = EventVisibility::Single;
    event->pos = pos;
}

void EventConfiguration::ConfigMouseEvent(MouseMessageTag<MouseMessage::Move>, std::shared_ptr<MouseEvent> event,
                                          IVec2 pos)
{
    event->visibility = EventVisibility::Single;
    event->pos = pos;
}

void EventConfiguration::ConfigMouseEvent(MouseMessageTag<MouseMessage::Scroll>, std::shared_ptr<MouseEvent> event,
                                          IVec2 pos, Vec2 offset)
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
