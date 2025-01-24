#include "Events.h"

using namespace Galvanizer;


std::string KeyEvent::strMessage() const
{
    switch (message)
    {
    case KeyMessage::Codepoint:
        return "KeyEvent::Codepoint";
    case KeyMessage::Key:
        return "KeyEvent::Key";

    default:
        return "KeyEvent::UnknownMessage";
    }
}

std::string MouseEvent::strMessage() const
{
    switch (message)
    {
    case MouseMessage::Button:
        return "MouseEvent::Button";
    case MouseMessage::Scroll:
        return "MouseEvent::Scroll";
    case MouseMessage::Move:
        return "MouseEvent::Move";
    case MouseMessage::Enter:
        return "MouseEvent::Enter";
    case MouseMessage::Leave:
        return "MouseEvent::Leave";

    default:
        return "MouseEvent::UnknownMessage";
    }
}


std::string ObjectEvent::strMessage() const
{
    switch (message)
    {
    case ObjectMessage::Init:
        return "ObjectEvent::Init";
    case ObjectMessage::Close:
        return "ObjectEvent::Close";
    case ObjectMessage::Terminate:
        return "ObjectEvent::Terminate";

    default:
        return "ObjectEvent::UnknownMessage";
    }
}


std::string WindowEvent::strMessage() const
{
    switch (message)
    {
    case WindowMessage::Iconify:
        return "WindowEvent::Iconify";
    case WindowMessage::Maximise:
        return "WindowEvent::Maximise";
    case WindowMessage::Restore:
        return "WindowEvent::Restore";
    case WindowMessage::Resize:
        return "WindowEvent::Resize";
    case WindowMessage::Render:
        return "WindowEvent::Render";
    case WindowMessage::Close:
        return "WindowEvent::Close";
    case WindowMessage::Position:
        return "WindowEvent::Position";
    case WindowMessage::Refresh:
        return "WindowEvent::Refresh";
    case WindowMessage::Undefined:
        return "WindowEvent::Undefined";
    case WindowMessage::GainFocus:
        return "WindowEvent::GainFocus";
    case WindowMessage::LoseFocus:
        return "WindowEvent::LoseFocus";

    case WindowMessage::CreateWindow:
        return "WindowEvent::CreateWindow";
    case WindowMessage::DestroyWindow:
        return "WindowEvent::DestroyWindow";
    case WindowMessage::RegisterHNDL:
        return "WindowEvent::RegisterHNDL";

    default:
        return "WindowEvent::UnknownMessage";
    }
}

std::string AppEvent::strMessage() const
{
    switch (message)
    {
    default:
        return "AppEvent::UnknownMessage";
    }
}

std::string ELEvent::strMessage() const
{
    switch (message)
    {
    case ELMessage::Run:
        return "ELMessage::Run";
    case ELMessage::Stop:
        return "ELMessage::Stop";

    default:
        return "ELMessage::UnknownMessage";
    }
}
