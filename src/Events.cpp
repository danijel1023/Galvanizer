#include "Events.h"

using namespace Galvanizer;


std::string KeyEvent::strMessage() const
{
    switch (message)
    {
    case KeyMessage::Text:
        return "KeyEvent::Text";
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
    case MouseMessage::Down:
        return "MouseEvent::Down";
    case MouseMessage::Up:
        return "MouseEvent::Up";
    case MouseMessage::ScrollUp:
        return "MouseEvent::ScrollUp";
    case MouseMessage::ScrollDown:
        return "MouseEvent::ScrollDown";
    case MouseMessage::ScrollLeft:
        return "MouseEvent::ScrollLeft";
    case MouseMessage::ScrollRight:
        return "MouseEvent::ScrollRight";
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

    default:
        return "WindowEvent::UnknownMessage";
    }
}

std::string GPUEvent::strMessage() const
{
    switch (message)
    {
    case GPUMessage::Init:
        return "GPUEvent::Init";
    case GPUMessage::Run:
        return "GPUEvent::Run";
    case GPUMessage::TerminateThread:
        return "GPUEvent::TerminateThread";
    case GPUMessage::Render:
        return "GPUEvent::Render";
    case GPUMessage::LoadTexture:
        return "GPUEvent::LoadTexture";

    default:
        return "GPUEvent::UnknownMessage";
    }
}

std::string AppEvent::strMessage() const
{
    switch (message)
    {
    case AppMessage::Nothing:
        return "AppEvent::Nothing";

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
