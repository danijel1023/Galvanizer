#pragma once

#include <string>
#include <mutex>
#include <vector>
#include <memory>

#include "units.h"


//[TODO] fix name
#define EVENT_REGISTRATION_INTERFACE(EventType) \
protected:\
friend class EventManager;\
friend class Event;\
inline static int _EventID = 0;\
public:\
static int GetEventID() { return _EventID; }\
EventType() { m_type = _EventID; }



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

class MainWindow;

using GObjHNDL = GalvanizerObject*;
using WinHNDL = MainWindow*;


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
        return m_EIDNames[EventType - 1];
    }


    template<class s>
    void RegisterEvent(std::string_view name)
    {
        // 'static' makes the 'GenerateID<s>(name)' run once per template type which makes
        // sure that there is no duplicate names and IDs for that type. This way we dont
        // have to check if we stored the name already and dont have to deal with typeid
        [[maybe_unused]] static bool dummy = GenerateID<s>(name);
    }

private:
    EventManager() = default;

    template<class s>
    bool GenerateID(std::string_view name)
    {
        std::lock_guard<std::mutex> lk(m_EIDMutex);

        s::_EventID = m_EIDCounter++;
        m_EIDNames.emplace_back(name);
        return true;
    }

    int m_EIDCounter = 1;
    std::mutex m_EIDMutex;
    std::vector<std::string> m_EIDNames;
};


enum class EventVisibility
{
    Unset, Global, Root, Single
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
    { return m_type == s::_EventID; }

    [[nodiscard]] int GetType() const
    { return m_type; }

    virtual std::string strMessage() = 0;

    EventVisibility visibility = {};
    ChildPriority priority = {};

    std::shared_ptr<Event> responseEvent;
    GObjHNDL receiver = nullptr;

protected:
    int m_type = 0;

    friend class EventManager;
};


enum class ObjectMessage
{
    Init, Run,
    // Do not rely on Close event being sent always. Use Terminate instead!
    Close,
    Terminate, Empty
};

struct ObjectEvent
    : public Event
{
EVENT_REGISTRATION_INTERFACE(ObjectEvent);

    std::string strMessage() override;

    ObjectMessage message = {};
    GObjHNDL objHndl = nullptr;
};


enum class ELMessage
{
    Run, Stop
};

struct ELEvent
    : public Event
{
EVENT_REGISTRATION_INTERFACE(ELEvent);

    std::string strMessage() override;

    ELMessage message = {};
};


enum class KeyAction
{
    Up, Down, Repeat
};

enum class KeyMessage
{
    Key, Text
};

struct KeyEvent
    : public Event
{
EVENT_REGISTRATION_INTERFACE(KeyEvent);

    std::string strMessage() override;

    KeyAction action = {};
    KeyMessage message = {};

    std::string str;
};


enum class MouseButton
{
    R, M, L
};

enum class MouseMessage
{
    Down, Up,

    ScrollUp, ScrollDown,
    ScrollLeft, ScrollRight,

    Move, Enter, Leave
};

struct MouseEvent
    : public Event
{
EVENT_REGISTRATION_INTERFACE(MouseEvent)

    std::string strMessage() override;

    MouseMessage message = {};
    MouseButton button = {};

    Vec2 pos;
    bool modAlt = false, modCtrl = false, modShift = false;
};


enum class WindowMessage
{
    Iconify, Maximise, Restore,
    Resize, Render
};

struct WindowEvent
    : public Event
{
EVENT_REGISTRATION_INTERFACE(WindowEvent)

    std::string strMessage() override;

    WindowMessage message = {};

    Vec2 pos, size;
    std::string name;
};


enum class GPUMessage
{
    Init, Run, TerminateThread,
    Render, LoadTexture
};

struct GPUEvent
    : public Event
{
EVENT_REGISTRATION_INTERFACE(GPUEvent)

    std::string strMessage() override;

    GPUMessage message = {};
};


enum class AppMessage
{
    Nothing
};

struct AppEvent
    : public Event
{
EVENT_REGISTRATION_INTERFACE(AppEvent)

    std::string strMessage() override;

    AppMessage message = {};

    void (* funcPtr)() = nullptr;

    WinHNDL winHndl = nullptr;
};


}
