#pragma once

#include <string>
#include <mutex>
#include <vector>
#include <memory>

#include "units.h"

//[TODO] fix name
//[TODO] IMPORTANT: var names cannot start with '_'
#define EVENT_REGISTRATION_INTERFACE(EventType) \
protected:\
friend class EventManager;\
friend class Event;\
inline static int EventID_ = 0;\
public:\
static int GetEventID() { return EventID_; }\
EventType() { m_type = EventID_; }



/*
  After thinking, event should be inherited since trying to work with
  composition would compose more problems than solutions. In this case,
  inheritance is just the better choice.

  Problems:
    - Destructor: after the event has been processed, it has to be deleted. Since the
      event loop doesn't know the type that Event holds (its just a void*), it has
      to have the destructor pointer to that class also

  Solutions:
    [none]
*/


namespace Galvanizer
{
class GalvanizerObject;
using GObj = GalvanizerObject;

class EventManager
{
public:
    EventManager(EventManager&&) = delete;

    static EventManager& get()
    {
        static EventManager em;
        return em;
    }

    std::string GetName(int EventType)
    {
        std::lock_guard lk(m_EIDMutex);
        if (EventType < m_EIDCounter && EventType)
            return m_EIDNames[EventType - 1];
        else
            return "[Unregistered Event Type]";
    }


    template<class s>
    void RegisterEvent(std::string_view name)
    {
        // 'static' makes the 'GenerateID<s>(name)' run once per template type which makes
        // sure that there are no duplicate names and IDs for that type. This way we dont
        // have to check if we stored the name already and dont have to deal with typeid
        [[maybe_unused]] static bool dummy = GenerateID<s>(name);
    }

private:
    EventManager() = default;

    template<class s>
    bool GenerateID(std::string_view name)
    {
        std::lock_guard lk(m_EIDMutex);

        s::EventID_ = m_EIDCounter++;
        m_EIDNames.emplace_back(name);
        return true;
    }

    int m_EIDCounter = 1;
    std::mutex m_EIDMutex;
    std::vector<std::string> m_EIDNames;
};


enum class EventVisibility
{
    Unset, Root, Single
};

enum class ChildPriority
{
    First, Last
};

class Event
{
public:
    virtual ~Event() = default;

    template<class s>
    [[nodiscard]] bool IsType() const
    {
        return m_type == s::EventID_;
    }

    [[nodiscard]] int GetType() const
    {
        return m_type;
    }

    [[nodiscard]] virtual std::string strMessage() const = 0;

    EventVisibility visibility = {};
    ChildPriority priority = {};

    std::shared_ptr<Event> responseEvent;
    std::weak_ptr<GObj> receiver;

    bool ignoreChildOnSeparateThread = false;

protected:
    int m_type = 0;

    friend class EventManager;
};


enum class ObjectMessage
{
    Undefined,

    Init,
    Close,  // Post it to the child's parent and with child as the argument. If there is no parent, post it to the child
    Terminate, Empty
};

struct ObjectEvent : Event
{
    EVENT_REGISTRATION_INTERFACE(ObjectEvent);

    [[nodiscard]] std::string strMessage() const override;

    ObjectMessage message = {};
    std::weak_ptr<GObj> objHndl;
};


enum class ELMessage
{
    Undefined,
    Run, Stop
};

struct ELEvent : Event
{
    EVENT_REGISTRATION_INTERFACE(ELEvent);

    [[nodiscard]] std::string strMessage() const override;

    ELMessage message = {};
};


enum class KeyAction
{
    Undefined,
    Press, Release, Repeat
};

enum class KeyButton
{
    Undefined, Unknown,
    NonUS1 = 161, NonUS2,

    Escape = 256, Enter, Tab, Backspace, Insert, Delete,
    Right, Left, Down, Up,

    PageUp, PageDown, Home, End,
    CapsLock = 280, ScrollLock, NumLock, PrintScreen, Pause,

    F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13,
    F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25,

    KP0 = 320, KP1, KP2, KP3, KP4, KP5, KP6, KP7, KP8, KP9,
    KPDecimal, KPDivide, KPMultiply, KPSubtract, KPAdd, KPEnter, KPEqual,

    LeftShift = 340, LeftControl, LeftAlt, LeftSuper,
    RightShift, RightControl, RightAlt, RightSuper,
    Menu,

    LowerBound = NonUS1, UpperBound = Menu
};

enum class KeyMessage
{
    Undefined,
    Key, Codepoint
};

struct KeyEvent : Event
{
    EVENT_REGISTRATION_INTERFACE(KeyEvent);

    [[nodiscard]] std::string strMessage() const override;

    KeyMessage message = {};
    KeyButton button = {};
    KeyAction action = {};

    int codepoint = 0;
    int scancode = 0;

    bool modAlt = false, modCtrl = false, modShift = false, modSuper = false, modCaps = false, modNum = false;
};


enum class MouseAction
{
    Undefined,
    Press, Release
};

enum class MouseButton
{
    Undefined,

    R, M, L,

    B1 = R, B2 = M, B3 = L,
    B4, B5, B6, B7, B8
};

enum class MouseMessage
{
    Undefined,

    Button, Scroll,
    Move, Enter, Leave
};

struct MouseEvent : Event
{
    EVENT_REGISTRATION_INTERFACE(MouseEvent)

    [[nodiscard]] std::string strMessage() const override;

    MouseMessage message = {};
    MouseButton button = {};
    MouseAction action = {};

    DVec2 scrollOffset;
    IVec2 pos;

    bool modAlt = false, modCtrl = false, modShift = false, modSuper = false, modCaps = false, modNum = false;
};


enum class WindowMessage
{
    Undefined,

    Maximise, Iconify, Restore,
    Resize, ResizeRequest, Position, PositionRequest,
    GainFocus, LoseFocus,
    Close, Refresh,

    RenderInit, RenderExit,
    Render, RenderRequest,

    CreateWindow, DestroyWindow,
    RegisterHNDL
};

struct WindowEvent : Event
{
    EVENT_REGISTRATION_INTERFACE(WindowEvent)

    [[nodiscard]] std::string strMessage() const override;

    WindowMessage message = {};

    void* winHNDL = nullptr;
    std::weak_ptr<GObj> objHndl;

    IVec2 pos, size;
    std::string name;
    void* share = nullptr;
    bool renderCorrection = false;
};


enum class CursorType
{
    Undefined,
    Arrow, IBeam, Crosshair, PointingHand,
    ResizeHorizontal, ResizeVertical, ResizeTLBR, ResizeTRBL, ResizeAll,
    NotAllowed,

    LowerBound = Arrow, UpperBound = NotAllowed
};

enum class AppMessage
{
    Undefined,
    SetCursor, TimedEvent
};

struct AppEvent : Event
{
    EVENT_REGISTRATION_INTERFACE(AppEvent)

    [[nodiscard]] std::string strMessage() const override;

    AppMessage message = {};
    CursorType cursorType = {};
    void* winHNDL = nullptr;

    std::weak_ptr<GObj> timerReceiver;
    std::chrono::steady_clock::time_point timeout = {};
};
}
