#include <string>
#include <vector>
#include <iostream>

#include "Application.h"
#include "MainWindow.h"
#include "Utility.h"
#include "EventConfigurations.h"

#include "GLFW/glfw3.h"

using namespace Galvanizer;
using namespace EventConfiguration;


Factory* Application::FindTarget(Factory* elm, std::string_view target) const
{
    if (!Utility::ValidateKeys(target))
    {
        std::cout << "[ERROR] ValidateKeys failed for: \"" << target << "\"" << std::endl;
        return nullptr;
    }

    std::vector<std::string> keys = Utility::ExtractKeys(target);
    if (keys.empty())
    {
        std::cout << "[WARN]: No keys extracted for \"" << target << "\"" << std::endl;
        return elm;
    }


    for (auto& key: keys)
    {
        bool found = false;
        for (const auto& elmCh: elm->children)
        {
            if (elmCh->name != key)
                continue;

            found = true;
            elm = elmCh;
            break;
        }

        if (!found) // Create new element with this key
        {
            elm->children.push_back(new Factory(key, elm));
            elm = elm->children.back();
        }
    }

    return elm;
}

Application::Application()
    : GalvanizerObject("app", WeakRef(), nullptr, false), m_eventLoop(&m_BO, this, false),
      m_ELRef(&m_eventLoop)
{
    m_ApplicationThis = this;

    p_displayName = "Application";
    p_eventLoopRef = &m_ELRef;

    EventManager& em = EventManager::get();
    em.RegisterEvent<KeyEvent>("KeyEvent");
    em.RegisterEvent<MouseEvent>("MouseEvent");
    em.RegisterEvent<WindowEvent>("WindowEvent");
    em.RegisterEvent<GPUEvent>("GPUEvent");
    em.RegisterEvent<AppEvent>("AppEvent");
    em.RegisterEvent<ObjectEvent>("ObjectEvent");

    ObjectFactories& of = ObjectFactories::GetInstance();
    of.CreateOwner("app");
    of.Get(pc_internalName);

    glfwInit();
}

Application::~Application()
{
    glfwTerminate();
}


Application& Application::get()
{
    return static_cast<Application&>(*m_ApplicationThis);
}


OwningRef Application::FindGObj(const std::string_view target)
{
    if (!Utility::ValidateKeys(target))
    {
        std::cout << "[ERROR] ValidateKeys failed for: \"" << target << "\"" << std::endl;
        return {};
    }

    const std::vector<std::string> keys = Utility::ExtractKeys(target);

    std::vector<OwningRef> obj;
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
    // Will NOT crash on exit if the application was made on stack - hotfix on OwningRef
    m_strongAppRef = CreateOwningRef(this);
    p_weakSelf = m_strongAppRef;

    const auto init = CreateObjectEvent<ObjectMessage::Init>();
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


static GLFWwindow* window;

uintptr_t Application::Callback(const std::shared_ptr<Event>& event)
{
    if (event->IsType<ObjectEvent>())
    {
        auto& objectEvent = static_cast<ObjectEvent&>(*event);
        switch (objectEvent.message)
        {
        case ObjectMessage::Init:
        {
            window = glfwCreateWindow(640, 480, "Application window2", nullptr, nullptr);
            glfwMakeContextCurrent(window);

            glfwSwapBuffers(window);
            glfwPollEvents();
            break;
        }

        case ObjectMessage::Close:
        {
            std::cout << "[INFO] Application got Close signal!" << std::endl;
            break;
        }

        case ObjectMessage::Terminate:
        {
            if (objectEvent.objHndl != this)
                break;

            std::cout << "[INFO] Application got Terminate signal." << std::endl;

            GalvanizerObject::Callback(event);

            p_eventLoopRef->Stop();
            m_strongAppRef.DropOwnership();

            glfwDestroyWindow(window);
            window = nullptr;
            return 0;
        }

        default:
            std::cout << "[ERROR] Application received unhandled event: " << event->strMessage() << std::endl;
            break;
        }
    }

    else if (event->IsType<AppEvent>())
    {
        auto& appEvent = static_cast<AppEvent&>(*event);
        switch (appEvent.message)
        {
        default:
            return -2;
        }
    }

    return GalvanizerObject::Callback(event);
}
