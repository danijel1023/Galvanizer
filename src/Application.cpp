#include <string>
#include <vector>
#include <iostream>

#include "Application.h"
#include "MainWindow.h"
#include "Utility.h"
#include "EventConfigurations.h"

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
    : GalvanizerObject("app", nullptr, nullptr),
      m_eventLoop(&m_BO, this, false), m_ELRef(&m_eventLoop)
{
    p_displayName = "Application";
    p_eventLoopRef = &m_ELRef;
    m_This = this;

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
}

Application::~Application()
{}


Application& Application::get()
{
    return *m_This;
}


GObjHNDL Application::FindGObj(std::string_view target)
{
    if (!Utility::ValidateKeys(target))
    {
        std::cout << "[ERROR] ValidateKeys failed for: \"" << target << "\"" << std::endl;
        return nullptr;
    }

    std::vector<std::string> keys = Utility::ExtractKeys(target);

    GObjHNDL obj = this;
    for (std::string_view key: keys)
    {
        obj = obj->FindChild(key);

        if (!obj)
        {
            std::cout << "[INFO] No window object found at \"" << key << "\" in \"" << target << "\"" << std::endl;
            return nullptr;
        }
    }

    return obj;
}


int Application::Run()
{
    auto init = CreateObjectEvent<ObjectMessage::Init>();
    PostEvent(init);

    auto run = CreateObjectEvent<ObjectMessage::Run>();
    PostEvent(run);

    m_eventLoop.Start();
    return 0;
}


uintptr_t Application::Dispatcher(std::shared_ptr<Event> event)
{
    return GalvanizerObject::Dispatcher(event);
}

uintptr_t Application::Callback(std::shared_ptr<Event> event)
{
    if (event->IsType<ObjectEvent>())
    {
        auto& objectEvent = static_cast<ObjectEvent&>(*event);
        switch (objectEvent.message)
        {
        case ObjectMessage::Close:
        {
            std::cout << "[INFO] Application got Close signal!" << std::endl;
            break;
        }

        case ObjectMessage::Terminate:
        {
            if (objectEvent.objHndl != this)
                return GalvanizerObject::Callback(event);

            std::cout << "[INFO] Application got Terminate signal." << std::endl;

            auto terminate = EventConfiguration::CreateObjectEvent<ObjectMessage::Terminate>(nullptr);
            for (auto& ch: p_children)
            {
                terminate->objHndl = ch;
                PostEvent(terminate);
            }

            m_eventLoop.Stop();
            return 0;
        }

        default:
            //std::cout << "[ERROR] Application received unhandled event: " << event->strMessage() << std::endl;
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
