#pragma once
#include <cmath>

#include "GLFW/glfw3.h"
#include "Events.h"


// @formatter:off

namespace Galvanizer
{
static void GLFWKeyCallback(GLFWwindow* winHndl, int key, int scancode, int action, int mods)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
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

    This->PostEvent(event, This);
}

static void GLFWCharCallback(GLFWwindow* winHndl, unsigned int codePoint)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;

    This->PostEvent(EventConfiguration::CreateKeyEvent<KeyMessage::Codepoint>(codePoint), This);
}



static void GLFWCursorPosCallback(GLFWwindow* winHndl, double xPos, double yPos)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    DVec2 pos(xPos, yPos);

    IVec2 winSize;
    glfwGetWindowSize(winHndl, &winSize.x, &winSize.y);
    pos = {pos.x ,winSize.y - pos.y};

    Vec2 scale;
    glfwGetWindowContentScale(winHndl, &scale.x, &scale.y);
    Vec2 scaledPos = Utility::PlatformScaleDown(Vec2(pos.x, pos.y), scale);

    This->PostEvent(EventConfiguration::CreateMouseEvent<MouseMessage::Move>(scaledPos), This);
}

static void GLFWCursorEnterCallback(GLFWwindow* winHndl, int entered)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;

    if (entered)
        This->PostEvent(EventConfiguration::CreateMouseEvent<MouseMessage::Enter>(), This);
    else
        This->PostEvent(EventConfiguration::CreateMouseEvent<MouseMessage::Leave>(), This);
}

static void GLFWMouseButtonCallback(GLFWwindow* winHndl, int button, int action, int mods)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
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
    default: return;    // Not supported
    }

    MouseAction ma;
    if (action == GLFW_PRESS)
        ma = MouseAction::Press;
    else if (action == GLFW_RELEASE)
        ma = MouseAction::Release;
    else
        ma = MouseAction::Undefined;

    DVec2 pos;
    glfwGetCursorPos(winHndl, &pos.x, &pos.y);

    IVec2 winSize;
    glfwGetWindowSize(winHndl, &winSize.x, &winSize.y);
    pos = {pos.x ,winSize.y - pos.y};

    Vec2 scale;
    glfwGetWindowContentScale(winHndl, &scale.x, &scale.y);
    Vec2 scaledPos = Utility::PlatformScaleDown(Vec2(pos.x, pos.y), scale);

    auto event = EventConfiguration::CreateMouseEvent<MouseMessage::Button>(scaledPos, mb, ma);

    event->modShift = mods & GLFW_MOD_SHIFT;
    event->modAlt   = mods & GLFW_MOD_ALT;
    event->modCtrl  = mods & GLFW_MOD_CONTROL;
    event->modSuper = mods & GLFW_MOD_SUPER;
    event->modCaps  = mods & GLFW_MOD_CAPS_LOCK;
    event->modNum   = mods & GLFW_MOD_NUM_LOCK;

    This->PostEvent(event, This);
}

static void GLFWScrollCallback(GLFWwindow* winHndl, double xOffset, double yOffset)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    DVec2 pos;
    glfwGetCursorPos(winHndl, &pos.x, &pos.y);

    IVec2 winSize;
    glfwGetWindowSize(winHndl, &winSize.x, &winSize.y);
    pos = {pos.x ,winSize.y - pos.y};

    Vec2 scale;
    glfwGetWindowContentScale(winHndl, &scale.x, &scale.y);
    Vec2 scaledPos = Utility::PlatformScaleDown(Vec2(pos.x, pos.y), scale);

    This->PostEvent(EventConfiguration::CreateMouseEvent<MouseMessage::Scroll>(scaledPos, Vec2(xOffset, yOffset)), This);
}



static void GLFWWindowSizeCallback(GLFWwindow* winHndl, int width, int height)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    Vec2 scale;
    glfwGetWindowContentScale(winHndl, &scale.x, &scale.y);
    Vec2 size = Utility::PlatformScaleDown(Vec2(width, height), scale);
    This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Resize>(This, size), This);
}

static void GLFWFramebufferSizeCallback(GLFWwindow* winHndl, int width, int height)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::FBResize>(This, Vec2(width, height)), This);
}

static void GLFWWindowScaleCallback(GLFWwindow* winHndl, float xScale, float yScale)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Scale>(Vec2(xScale, yScale)), This);
}

static void GLFWWindowFocusCallback(GLFWwindow* winHndl, int focused)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    if (focused)
        This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::GainFocus>(), This);
    else
        This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::LoseFocus>(), This);
}

static void GLFWWindowCloseCallback(GLFWwindow* winHndl)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Close>(), This);
}

static void GLFWWindowMaximizeCallback(GLFWwindow* winHndl, int maximized)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    if (maximized)
        This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Maximise>(), This);
    else
        This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Restore>(), This);
}

static void GLFWWindowIconifyCallback(GLFWwindow* winHndl, int iconified)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    if (iconified)
        This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Iconify>(), This);
    else
        This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Restore>(), This);
}

static void GLFWWindowRefreshCallback(GLFWwindow* winHndl)
{
    auto This = static_cast<std::weak_ptr<GObj>*>(glfwGetWindowUserPointer(winHndl))->lock();
    if (!This)
        return;


    This->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Refresh>(), This);
}
}
