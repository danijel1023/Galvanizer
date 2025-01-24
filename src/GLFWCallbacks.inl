#pragma once
#include <cmath>

// @formatter:off

namespace Galvanizer
{
static void GLFWKeyCallback(GLFWwindow* winHndl, int key, int scancode, int action, int mods)
{
    auto This = static_cast<WeakRef*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;

    KeyButton kb;
    if (key == GLFW_KEY_UNKNOWN)
        kb = KeyButton::Unknown;
    else
        kb = static_cast<KeyButton>(key);

    KeyAction ka;
    if (action == GLFW_PRESS)
        ka = KeyAction::Press;
    else if (action == GLFW_RELEASE)
        ka = KeyAction::Release;
    else if (action == GLFW_REPEAT)
        ka = KeyAction::Repeat;
    else
        ka = KeyAction::Undefined;

    auto event = EventConfiguration::CreateKeyEvent<KeyMessage::Key>(kb, scancode, ka);

    event->modShift = mods & GLFW_MOD_SHIFT;
    event->modAlt   = mods & GLFW_MOD_ALT;
    event->modCtrl  = mods & GLFW_MOD_CONTROL;
    event->modSuper = mods & GLFW_MOD_SUPER;
    event->modCaps  = mods & GLFW_MOD_CAPS_LOCK;
    event->modNum   = mods & GLFW_MOD_NUM_LOCK;

    This->PostEvent(event);
}

static void GLFWCharCallback(GLFWwindow* winHndl, unsigned int codePoint)
{
    auto This = static_cast<WeakRef*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;

    This->PostEvent(EventConfiguration::CreateKeyEvent<KeyMessage::Codepoint>(codePoint));
}



static void GLFWCursorPosCallback(GLFWwindow* winHndl, double xPos, double yPos)
{
    auto This = static_cast<WeakRef*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    IVec2 pos(static_cast<int>(std::floor(xPos)), static_cast<int>(std::floor(yPos)));
    This->PostEvent(EventConfiguration::CreateMouseEvent<MouseMessage::Move>(pos));
}

static void GLFWCursorEnterCallback(GLFWwindow* winHndl, int entered)
{
    auto This = static_cast<WeakRef*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    Vec2 DPos;
    glfwGetCursorPos(winHndl, &DPos.x, &DPos.y);
    IVec2 pos(static_cast<int>(std::floor(DPos.x)), static_cast<int>(std::floor(DPos.y)));

    if (entered)
        This->PostEvent(EventConfiguration::CreateMouseEvent<MouseMessage::Enter>(pos));
    else
        This->PostEvent(EventConfiguration::CreateMouseEvent<MouseMessage::Leave>(pos));
}

static void GLFWMouseButtonCallback(GLFWwindow* winHndl, int button, int action, int mods)
{
    auto This = static_cast<WeakRef*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    MouseButton mb;
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:    mb = MouseButton::L;  break;
    case GLFW_MOUSE_BUTTON_RIGHT:   mb = MouseButton::R;  break;
    case GLFW_MOUSE_BUTTON_MIDDLE:  mb = MouseButton::M;  break;
    case GLFW_MOUSE_BUTTON_4:       mb = MouseButton::B4; break;
    case GLFW_MOUSE_BUTTON_5:       mb = MouseButton::B5; break;
    case GLFW_MOUSE_BUTTON_6:       mb = MouseButton::B6; break;
    case GLFW_MOUSE_BUTTON_7:       mb = MouseButton::B7; break;
    case GLFW_MOUSE_BUTTON_8:       mb = MouseButton::B8; break;
    default: return;    //Not supported
    }

    MouseAction ma;
    if (action == GLFW_PRESS)
        ma = MouseAction::Press;
    else if (action == GLFW_RELEASE)
        ma = MouseAction::Release;
    else
        ma = MouseAction::Undefined;

    Vec2 DPos;
    glfwGetCursorPos(winHndl, &DPos.x, &DPos.y);
    IVec2 pos(static_cast<int>(std::floor(DPos.x)), static_cast<int>(std::floor(DPos.y)));

    auto event = EventConfiguration::CreateMouseEvent<MouseMessage::Button>(pos, mb, ma);

    event->modShift = mods & GLFW_MOD_SHIFT;
    event->modAlt   = mods & GLFW_MOD_ALT;
    event->modCtrl  = mods & GLFW_MOD_CONTROL;
    event->modSuper = mods & GLFW_MOD_SUPER;
    event->modCaps  = mods & GLFW_MOD_CAPS_LOCK;
    event->modNum   = mods & GLFW_MOD_NUM_LOCK;

    This->PostEvent(event);
}

static void GLFWScrollCallback(GLFWwindow* winHndl, double xOffset, double yOffset)
{
    auto This = static_cast<WeakRef*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    Vec2 DPos;
    glfwGetCursorPos(winHndl, &DPos.x, &DPos.y);
    IVec2 pos(static_cast<int>(std::floor(DPos.x)), static_cast<int>(std::floor(DPos.y)));

    This->PostEvent(EventConfiguration::CreateMouseEvent<MouseMessage::Scroll>(pos, Vec2(xOffset, yOffset)));
}



static void GLFWWindowSizeCallback(GLFWwindow* winHndl, int width, int height)
{
    auto This = static_cast<WeakRef*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Resize>(IVec2(width, height)));
}

static void GLFWWindowPosCallback(GLFWwindow* winHndl, int xPos, int yPos)
{
    auto This = static_cast<WeakRef*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Position>(IVec2(xPos, yPos)));
}

static void GLFWWindowFocusCallback(GLFWwindow* winHndl, int focused)
{
    auto This = static_cast<WeakRef*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    if (focused)
        This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::GainFocus>());
    else
        This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::LoseFocus>());
}

static void GLFWWindowCloseCallback(GLFWwindow* winHndl)
{
    auto This = static_cast<WeakRef*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Close>());
}

static void GLFWWindowMaximizeCallback(GLFWwindow* winHndl, int maximized)
{
    auto This = static_cast<WeakRef*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    if (maximized)
        This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Maximise>());
    else
        This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Restore>());
}

static void GLFWWindowIconifyCallback(GLFWwindow* winHndl, int iconified)
{
    auto This = static_cast<WeakRef*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    if (iconified)
        This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Iconify>());
    else
        This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Restore>());
}

static void GLFWWindowRefreshCallback(GLFWwindow* winHndl)
{
    auto This = static_cast<WeakRef*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Refresh>());
}
}
