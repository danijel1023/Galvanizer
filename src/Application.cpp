#include <string>
#include <vector>
#include <iostream>

#include "Application.h"

#include "MainWindow.h"
#include "Utility.h"
#include "EventConfigurations.h"

#include "GLFW/glfw3.h"
#include "GLFWCallbacks.inl"


using namespace Galvanizer;
using namespace std::chrono_literals;

namespace
{
struct Application_Shared : Application
{
    template<typename... Args>
    Application_Shared(Args&&... args): Application(std::forward<Args>(args)...) {}
};
}

void Application::Init()
{
    m_ApplicationThis = std::make_shared<Application_Shared>();
    m_ApplicationThis->p_weakSelf = m_ApplicationThis;
}


std::shared_ptr<Application> Application::get()
{
    return m_ApplicationThis;
}


Application::Application()
    : GalvanizerObject("app", std::weak_ptr<GObj>(), nullptr), m_eventLoop(&m_BO, false),
      m_ELRef(&m_eventLoop)
{
    p_displayName = "Application";
    p_eventLoopRef = &m_ELRef;

    EventManager& em = EventManager::get();
    em.RegisterEvent<KeyEvent>("KeyEvent");
    em.RegisterEvent<MouseEvent>("MouseEvent");
    em.RegisterEvent<WindowEvent>("WindowEvent");
    em.RegisterEvent<AppEvent>("AppEvent");
    em.RegisterEvent<ObjectEvent>("ObjectEvent");

    ObjectFactories& of = ObjectFactories::GetInstance();
    of.CreateOwner("app");
    of.Get(pc_internalName);

    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    //glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    glfwInit();

    m_cursors[static_cast<int>(CursorType::Arrow) - 1] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    m_cursors[static_cast<int>(CursorType::IBeam) - 1] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    m_cursors[static_cast<int>(CursorType::Crosshair) - 1] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    m_cursors[static_cast<int>(CursorType::PointingHand) - 1] = glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR);
    m_cursors[static_cast<int>(CursorType::ResizeHorizontal) - 1] = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
    m_cursors[static_cast<int>(CursorType::ResizeVertical) - 1] = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
    m_cursors[static_cast<int>(CursorType::ResizeTLBR) - 1] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    m_cursors[static_cast<int>(CursorType::ResizeTRBL) - 1] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
    m_cursors[static_cast<int>(CursorType::ResizeAll) - 1] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
    m_cursors[static_cast<int>(CursorType::NotAllowed) - 1] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);

    switch (glfwGetPlatform())
    {
    case GLFW_PLATFORM_X11:
        m_platform = Platform::X11;
        break;

    case GLFW_PLATFORM_WAYLAND:
        m_platform = Platform::Wayland;
        break;

    case GLFW_PLATFORM_COCOA:
        m_platform = Platform::MacOS;
        break;

    case GLFW_PLATFORM_WIN32:
        m_platform = Platform::Windows;
        break;

    case GLFW_PLATFORM_NULL:
        m_platform = Platform::Null;
        break;


    default:
        std::cout << "[ERROR] Something serious went wrong: unknown glfwGetPlatform()" << std::endl;
    }
}

Application::~Application()
{
    glfwTerminate();
}



std::shared_ptr<GObj> Application::FindGObj(const std::string_view target)
{
    if (!Utility::ValidateKeys(target))
    {
        std::cout << "[ERROR] ValidateKeys failed for: \"" << target << "\"" << std::endl;
        return {};
    }

    const std::vector<std::string> keys = Utility::ExtractKeys(target);

    std::vector<std::shared_ptr<GObj>> obj;
    obj.reserve(keys.size());

    obj.emplace_back(p_weakSelf.lock());
    for (const std::string_view key: keys)
    {
        obj.emplace_back(obj.back()->FindChild(key).lock());

        if (!obj.back())
        {
            std::cout << "[INFO] No window object found at \"" << key << "\" in \"" << target << "\"" << std::endl;
            return {};
        }
    }

    return obj.back();
}


int Application::Run()
{
    const auto init = EventConfiguration::CreateObjectEvent<ObjectMessage::Init>();
    PostEvent(init);

    // Blocking call
    m_eventLoop.Start();

    // Applications event loop stopped
    std::cout << "[INFO] Exited application event loop." << std::endl;
    return 0;
}


uintptr_t Application::Dispatcher(const std::shared_ptr<Event>& event)
{
    return GalvanizerObject::Dispatcher(event);
}


uintptr_t Application::Callback(const std::shared_ptr<Event>& event)
{
    if (event->IsType<ObjectEvent>())
    {
        auto& objectEvent = static_cast<ObjectEvent&>(*event);
        switch (objectEvent.message)
        {
        case ObjectMessage::Init:
        {
            m_timerRunning = true;
            m_timerThread = std::thread(&Application::TimerLoop, this);

            break;
        }

        case ObjectMessage::Close:
        {
            std::cout << "[INFO] Application got Close signal!" << std::endl;

            m_timerRunning = false;
            m_timerThread.join();

            break;
        }

        case ObjectMessage::Terminate:
        {
            if (objectEvent.objHndl.lock() != p_weakSelf.lock())
                break;

            std::cout << "[INFO] Application got Terminate signal." << std::endl;

            GalvanizerObject::Callback(event);

            p_eventLoopRef->Stop();
            m_ApplicationThis.reset();

            return 0;
        }

        default:
            //std::cout << "[ERROR] Application received unhandled event: " << event->strMessage() << std::endl;
            break;
        }
    }

    else if (event->IsType<WindowEvent>())
    {
        auto& winEvent = static_cast<WindowEvent&>(*event);
        switch (winEvent.message)
        {
        case WindowMessage::CreateWindow:
        {
            if (auto obj = winEvent.objHndl.lock())
            {
                glfwWindowHint(GLFW_SAMPLES, 32);

                auto winHNDL = glfwCreateWindow(854, 480, winEvent.name.c_str(), nullptr,
                                                static_cast<GLFWwindow*>(winEvent.share));

                glfwSetInputMode(winHNDL, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

                m_winHNDLs.emplace_back(obj, winHNDL);
                glfwSetWindowUserPointer(winHNDL, &m_winHNDLs.back().first);

                glfwSetKeyCallback(winHNDL, &GLFWKeyCallback);
                glfwSetCharCallback(winHNDL, &GLFWCharCallback);

                glfwSetCursorPosCallback(winHNDL, &GLFWCursorPosCallback);
                glfwSetCursorEnterCallback(winHNDL, &GLFWCursorEnterCallback);
                glfwSetMouseButtonCallback(winHNDL, &GLFWMouseButtonCallback);
                glfwSetScrollCallback(winHNDL, &GLFWScrollCallback);

                glfwSetWindowSizeCallback(winHNDL, &GLFWWindowSizeCallback);
                glfwSetWindowFocusCallback(winHNDL, &GLFWWindowFocusCallback);
                glfwSetWindowCloseCallback(winHNDL, &GLFWWindowCloseCallback);
                glfwSetWindowMaximizeCallback(winHNDL, &GLFWWindowMaximizeCallback);
                glfwSetWindowIconifyCallback(winHNDL, &GLFWWindowIconifyCallback);
                glfwSetWindowRefreshCallback(winHNDL, &GLFWWindowRefreshCallback);
                glfwSetWindowContentScaleCallback(winHNDL, &GLFWWindowScaleCallback);
                glfwSetFramebufferSizeCallback(winHNDL, &GLFWFramebufferSizeCallback);

                obj->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::RegisterHNDL>(winHNDL));

                Vec2 scale;
                glfwGetWindowContentScale(winHNDL, &scale.x, &scale.y);
                obj->PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::Scale>(scale));

                Vec2 size = winEvent.size;
                size = Utility::PlatformScaleUp(size, scale);
                glfwSetWindowSize(winHNDL, size.x, size.y);
            }

            return 0;
        }

        case WindowMessage::DestroyWindow:
        {
            for (auto it = m_winHNDLs.begin(); it != m_winHNDLs.end(); it++)
            {
                if (it->first.lock() == winEvent.objHndl.lock())
                {
                    glfwDestroyWindow(static_cast<GLFWwindow*>(it->second));
                    m_winHNDLs.erase(it);

                    break;
                }
            }

            return 0;
        }

        case WindowMessage::Resize:
        {
            auto target = winEvent.objHndl.lock();
            if (target)
            {
                for (auto& winHNDL: m_winHNDLs)
                {
                    std::shared_ptr winObj = winHNDL.first.lock();
                    if (winObj == winEvent.objHndl.lock())
                    {
                        Vec2 scale;
                        glfwGetWindowContentScale(static_cast<GLFWwindow*>(winHNDL.second), &scale.x, &scale.y);

                        Vec2 size = winEvent.size;
                        size = Utility::PlatformScaleUp(size, scale);

                        glfwSetWindowSize(static_cast<GLFWwindow*>(winHNDL.second), size.x, size.y);
                        winObj->PostEvent(
                            EventConfiguration::CreateWindowEvent<WindowMessage::Resize>(winObj, winEvent.size));

                        break;
                    }
                }
            }

            return 0;
        }

        default:
            return -2;
        }
    }

    else if (event->IsType<AppEvent>())
    {
        auto& appEvent = static_cast<AppEvent&>(*event);
        switch (appEvent.message)
        {
        case AppMessage::SetCursor:
        {
            if (appEvent.cursorType <= CursorType::UpperBound && appEvent.cursorType >= CursorType::LowerBound)
                glfwSetCursor(static_cast<GLFWwindow*>(appEvent.winHNDL),
                              static_cast<GLFWcursor*>(m_cursors[static_cast<int>(appEvent.cursorType) - 1]));

            return 0;
        }

        case AppMessage::TimedEvent:
        {
            std::lock_guard lck(m_timerEventsMutex);
            m_timerEvents.emplace_back(event);

            return 0;
        }

        default:
            return -2;
        }
    }

    return GalvanizerObject::Callback(event);
}


void Application::TimerLoop()
{
    std::cout << "[DEBUG] Starting TimerLoop." << std::endl;

    while (m_timerRunning)
    {
        std::this_thread::sleep_for(1ms);

        std::lock_guard lck(m_timerEventsMutex);
        for (size_t i = 0; i < m_timerEvents.size(); i++)
        {
            auto& event = static_cast<AppEvent&>(*m_timerEvents[i]);
            if (event.timeout - std::chrono::steady_clock::now() <= 0ms)
            {
                if (std::shared_ptr obj = event.timerReceiver.lock())
                    obj->PostEvent(event.responseEvent);

                m_timerEvents.erase(m_timerEvents.begin() + i);
                i--;
            }
        }
    }

    std::cout << "[DEBUG] Exiting TimerLoop." << std::endl;
}
