#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

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
    : GalvanizerObject("ApplicationRoot", nullptr), m_eventLoop(&m_BO, this, false)
{
    EventManager& em = EventManager::get();
    em.RegisterEvent<KeyEvent>("KeyEvent");
    em.RegisterEvent<MouseEvent>("MouseEvent");
    em.RegisterEvent<WindowEvent>("WindowEvent");
    em.RegisterEvent<GPUEvent>("GPUEvent");
    em.RegisterEvent<AppEvent>("AppEvent");
    em.RegisterEvent<ObjectEvent>("ObjectEvent");

    p_eventLoopRef = &m_eventLoop;

    {
        FactoryRoot* defaultRoot = new FactoryRoot;
        defaultRoot->ownerName = "app";

        std::lock_guard lck(m_factoryMutex);
        m_factoryRoots.push_back(defaultRoot);
    }
}


Application& Application::get()
{
    static Application app;
    return app;
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
            std::cout << "[INFO] No window object found at \"" << key << "\" in \"" << target << "\"";
            return nullptr;
        }
    }

    return obj;
}

Factory* Application::FindFactory(std::string_view target, std::string_view owner)
{
    std::lock_guard lck(m_factoryMutex);

    size_t ownerIndex = 0;
    for (; ownerIndex < m_factoryRoots.size(); ownerIndex++)
        if (m_factoryRoots[ownerIndex]->ownerName == owner)
            break;


    if (ownerIndex == m_factoryRoots.size())
    {
        std::cout << "[ERROR] Couldn't Find ownerName: " << owner << std::endl;
        return nullptr;
    }

    if (!Utility::ValidateKeys(target))
    {
        std::cout << "[ERROR] ValidateKeys failed for: \"" << target << "\"" << std::endl;
        return nullptr;
    }

    std::vector<std::string> keys = Utility::ExtractKeys(target);

    Factory* currentFactory = &m_factoryRoots[ownerIndex]->factory;
    for (const auto& key: keys)
    {
        currentFactory = currentFactory->Find(key);

        if (!currentFactory)
        {
            std::cout << "[INFO] No factory object found at \"" << key << "\" in \"" << target << "\"";
            return nullptr;
        }
    }

    return currentFactory;
}

Factory* Application::GetFactory(std::string_view target, std::string_view owner)
{
    std::lock_guard lck(m_factoryMutex);

    size_t ownerIndex = 0;
    for (; ownerIndex < m_factoryRoots.size(); ownerIndex++)
        if (m_factoryRoots[ownerIndex]->ownerName == owner)
            break;

    if (ownerIndex == m_factoryRoots.size())
    {
        std::cout << "[INFO] Couldn't Find ownerName \"" << owner << "\". Creating new root..." << std::endl;
        FactoryRoot* newRoot = new FactoryRoot;
        newRoot->ownerName = owner;

        m_factoryRoots.push_back(newRoot);
    }

    if (!Utility::ValidateKeys(target))
    {
        std::cout << "[ERROR] ValidateKeys failed for: \"" << target << "\"" << std::endl;
        return nullptr;
    }

    std::vector<std::string> keys = Utility::ExtractKeys(target);

    Factory* currentFactory = &m_factoryRoots[ownerIndex]->factory;
    for (const auto& key: keys)
    {
        Factory* chFactory = currentFactory->Find(key);

        if (!chFactory)
        {
            std::cout << "[INFO] Creating factory \"" << key << "\" at namespace \"" << currentFactory->name
                      << "\" at owner \"" << *(currentFactory->rootOwner) << "\" for target \"" << target << "\""
                      << std::endl;

            chFactory = new Factory(key, currentFactory);
        }

        currentFactory = chFactory;
    }

    std::cout << "[INFO] Resetting factory: " << target << std::endl;

    // Reset values
    currentFactory->active = true;
    currentFactory->built = false;
    currentFactory->ptr = nullptr;

    return currentFactory;
}


std::vector<GObjHNDL> Application::Build(std::string_view path)
{
    std::lock_guard lck(m_factoryMutex);

    if (!Utility::ValidateKeys(path))
    {
        std::cout << "[ERROR] Validate keys failed for \"" << path << "\" in build" << std::endl;
        return {};
    }

    std::cout << "[INFO] Build namespace \"" << path << "\"" << std::endl;

    // [Factory, override factory's index]
    std::vector<std::pair<Factory*, size_t>> factories;

    // For each ownerName-
    for (const auto& owner: m_factoryRoots)
    {
        // Locate the "parent" factory whose children will get built
        Factory* parent = FindFactory(path, owner->ownerName);

        if (!parent)
            continue;

        // Add children to the list
        for (const auto& ch: parent->children)
        {
            if (!ch->built && ch->active && ch->ptr)
                factories.emplace_back(ch, -1);
        }
    }


    // Create "overrides owner" linked list with indices (factories[x].second)
    for (size_t current = 0; current < factories.size(); current++)
    {
        /*
        std::cout << "Factory(" << current << "): " << std::endl;
        std::cout << "- name:      " << factories[current].first->name << std::endl;
        std::cout << "- built:     " << Utility::strBool(factories[current].first->built) << std::endl;
        std::cout << "- ptr:       " << reinterpret_cast<void*>(factories[current].first->ptr) << std::endl;
        std::cout << "- rootOwner: " << *factories[current].first->rootOwner << std::endl;
        std::cout << "- active:    " << Utility::strBool(factories[current].first->active) << std::endl;
        std::cout << "- overrides: " << factories[current].first->overridesOwner << std::endl;
        */

        if (factories[current].first->overridesOwner.empty())
            continue;

        for (size_t overrideElm = 0; overrideElm < factories.size(); overrideElm++)
        {
            if (factories[current].first->overridesOwner == *factories[overrideElm].first->rootOwner)
            {
                if (factories[overrideElm].second != static_cast<size_t>(-1))
                {
                    std::cout << "[ERROR] Two or more factories are trying to override same factory: "
                              << factories[current].first->name << std::endl;

                    return {};
                }

                factories[overrideElm].second = current;
                break;
            }
        }
    }


    // factories: [ptr, index]
    // 'index' is an index into the 'factories' representing which factory is it overriding
    // Find loop in the list using Floyd’s Cycle-Finding Algorithm
    // But holy fk this is ugly...
    for (size_t i = 0; i < factories.size(); i++)
    {
        size_t fastPtr = factories[i].second;
        size_t slowPtr = factories[i].second;

        while (fastPtr != static_cast<size_t>(-1)
               && slowPtr != static_cast<size_t>(-1)
               && factories[fastPtr].second != static_cast<size_t>(-1))
        {
            slowPtr = factories[slowPtr].second;
            fastPtr = factories[factories[fastPtr].second].second;

            if (slowPtr == fastPtr)
            {
                // [TODO] Add recovery possibility - remove the loop and continue executing
                std::cout << "[ERROR] Loop dependency detected in path \"" << path << "\"" << std::endl;
                return {};
            }
        }
    }

    // Remove factory pointed by overridesOwner
    for (auto& fac: factories)
    {
        if (fac.second == static_cast<size_t>(-1))
            continue;

        factories[fac.second].first = nullptr;
    }


    // Print some nice information
    std::cout << "[INFO] Survived factories: ";
    bool firstPrint = true;
    for (auto WF: factories)
    {
        if (!WF.first)
            continue;

        if (!firstPrint)
            std::cout << ", ";

        std::cout << "[" << WF.first->name << ", " << WF.second << "]";
        firstPrint = false;
    }
    std::cout << std::endl;


    GObjHNDL parent = FindGObj(path);

    // Build the actual objects
    std::vector<GObjHNDL> builtObjects;
    for (auto& WF: factories)
    {
        if (WF.first && WF.first->ptr)
        {
            // Call factory (pointer)
            GObjHNDL hndl = WF.first->ptr(WF.first->name, parent);

            // Add the object to the parent
            parent->p_children.push_back(hndl);

            // Record the newly created children
            // (im getting on FBI watchlist for this many occurrences of word "children")
            builtObjects.push_back(hndl);

            WF.first->built = true;
        }
    }

    return builtObjects;
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
        case ObjectMessage::Init:
        {
            break;
        }

        case ObjectMessage::Close:
        {
            std::cout << "[INFO] Closing application." << std::endl;

            if (p_children.empty())
            {
                m_eventLoop.Stop();
            }

            else
            {
                for (auto& ch: p_children)
                {
                    auto close = CreateObjectEvent<ObjectMessage::Close>();
                    ch->PostEvent(close);
                }
            }

            m_closing = true;
            break;
        }

        case ObjectMessage::Terminate:
        {
            GalvanizerObject::Callback(event);

            if (m_closing && p_children.empty())
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

bool Application::Deconstruct(GObjHNDL obj)
{
    std::lock_guard lck(m_factoryMutex);

    std::string targetLocation = obj->getTarget();

    bool foundFac = false;
    for (const auto& owner: m_factoryRoots)
    {
        Factory* targetFac = FindFactory(targetLocation, owner->ownerName);

        if (!targetFac)
            continue;

        targetFac->built = false;
        foundFac = true;
    }

    if (obj->p_parent)
    {
        // Remove child object from the parent
        auto& parentCh = obj->p_parent->p_children;
        for (size_t i = 0; i < parentCh.size(); i++)
        {
            if (parentCh[i] != obj)
                continue;

            parentCh.erase(parentCh.begin() + i);
            break;
        }
    }

    delete obj;
    return foundFac;
}
