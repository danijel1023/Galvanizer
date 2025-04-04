#include <iostream>

#include "Factory.h"
#include "Utility.h"
#include "GalvanizerObject.h"
#include "EventConfigurations.h"

using namespace Galvanizer;


Factory::Factory(std::string_view name, Factory* parent)
    : name(name), parent(parent), rootOwner(parent->rootOwner)
{
    parent->children.push_back(this);
}

Factory::Factory(std::string_view name, std::string* rootOwner)
    : name(name), rootOwner(rootOwner) {}

Factory::~Factory()
{
    for (auto& ch: children)
        delete ch;
}


Factory* Factory::FindChild(std::string_view facName)
{
    for (const auto& ch: children)
        if (ch->name == facName)
            return ch;

    return nullptr;
}

void Factory::Reset()
{
    active = true;
    built = false;
    ptr = nullptr;
}


FactoryRoot::FactoryRoot(std::string_view ownerName)
    : ownerName(ownerName) {}


static ObjectFactories* s_ObjFacInstance = nullptr;

ObjectFactories::ObjectFactories() = default;

ObjectFactories& ObjectFactories::GetInstance()
{
    if (!s_ObjFacInstance)
    {
        std::cout << "[ERROR] ObjectFactories uninitialized in GetInstance - crash imminent" << std::endl;
        throw;
    }

    return *s_ObjFacInstance;
}


void ObjectFactories::Init()
{
    if (s_ObjFacInstance)
        std::cout << "[WARN] Re-initializing ObjectFactories - ignoring the request" << std::endl;
    else
        s_ObjFacInstance = new ObjectFactories();
}

void ObjectFactories::Shutdown()
{
    if (!s_ObjFacInstance)
        std::cout << "[WARN] Deleting NULL ObjectFactories - ignoring the request" << std::endl;
    else
        delete s_ObjFacInstance;

    s_ObjFacInstance = nullptr;
}

Factory* ObjectFactories::Find(const std::string_view target, const std::string_view owner)
{
    std::lock_guard lck(m_accessMutex);

    if (!Utility::ValidateKeys(target))
    {
        std::cout << "[ERROR] ValidateKeys failed for: \"" << target << "\"" << std::endl;
        return nullptr;
    }

    size_t ownerIndex = 0;
    for (; ownerIndex < m_facRoots.size(); ownerIndex++)
    {
        if (m_facRoots[ownerIndex]->ownerName == owner)
            break;
    }

    if (ownerIndex == m_facRoots.size())
    {
        std::cout << "[ERROR] Couldn't find ownerName: " << owner << std::endl;
        return nullptr;
    }


    std::vector<std::string> keys = Utility::ExtractKeys(target);
    if (keys.empty())
    {
        std::cout << "[WARN] No keys provided" << std::endl;
        return nullptr;
    }

    Factory* currentFactory = &m_facRoots[ownerIndex]->factory;
    for (const auto& key: keys)
    {
        currentFactory = currentFactory->FindChild(key);

        if (!currentFactory)
        {
            //std::cout << "[INFO] No factory object found at \"" << key << "\" in \"" << target << "\" with owner " <<
            //        owner << std::endl;
            return nullptr;
        }
    }

    return currentFactory;
}


Factory* ObjectFactories::Get(const std::string_view target)
{
    return Get(target, m_defaultOwnerName);
}

Factory* ObjectFactories::Get(const std::string_view target, const std::string_view owner)
{
    std::lock_guard lck(m_accessMutex);

    if (!Utility::ValidateKeys(target))
    {
        std::cout << "[ERROR] ValidateKeys failed for: \"" << target << "\"" << std::endl;
        return nullptr;
    }

    size_t ownerIndex = 0;
    for (; ownerIndex < m_facRoots.size(); ownerIndex++)
    {
        if (m_facRoots[ownerIndex]->ownerName == owner)
            break;
    }

    if (ownerIndex == m_facRoots.size())
    {
        std::cout << "[ERROR] Couldn't find ownerName: " << owner << std::endl;
        return nullptr;
    }


    std::vector<std::string> keys = Utility::ExtractKeys(target);


    Factory* currentFactory = &m_facRoots[ownerIndex]->factory;
    //std::cout << "[INFO] For target \"" << *(currentFactory->rootOwner) << ":" << target << "\" -----" << std::endl;

    for (const auto& key: keys)
    {
        Factory* chFactory = currentFactory->FindChild(key);

        if (!chFactory)
        {
            //std::cout << "Creating factory \"" << key << "\" at namespace \"" << currentFactory->name << "\"" <<
            //        std::endl;

            chFactory = new Factory(key, currentFactory);
        }

        currentFactory = chFactory;
    }

    //std::cout << std::endl;

    return currentFactory;
}

std::vector<std::string_view> ObjectFactories::GetFacNames(std::string_view path)
{
    std::cout << "[ERROR] GetFacNames in ObjectFactories was not implemented" << std::endl;
    return {};
}


Factory* ObjectFactories::FindTarget(Factory* elm, std::string_view target)
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


std::vector<std::shared_ptr<GObj>> ObjectFactories::Build(const std::shared_ptr<GObj>& pathObj)
{
    if (pathObj->Closing())
        return {}; // Drop build requests if its closing

    std::lock_guard lck(m_accessMutex);

    const std::string path = pathObj->GetTarget();
    //std::cout << "[INFO] Build namespace \"" << path << "\"" << std::endl;

    struct FacListNode
    {
        Factory* fac = nullptr;
        size_t index = -1;
    };

    std::vector<FacListNode> factories;

    for (const auto& owner: m_facRoots)
    {
        // FindChild the factory corresponding to "path" (current 'parent')
        Factory* parent = Find(path, owner->ownerName);

        // Couldn't find the parent - skip
        if (!parent)
            continue;

        // Add all children the paren has; per owner
        for (const auto& ch: parent->children)
        {
            if (!ch->built && ch->active && ch->ptr)
                factories.emplace_back(ch, -1);
        }
    }

    if (factories.empty())
        return {};

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

        if (factories[current].fac->overridesOwner.empty())
            continue;

        for (size_t overrideElm = 0; overrideElm < factories.size(); overrideElm++)
        {
            // Find a name match
            if (factories[current].fac->name != factories[overrideElm].fac->name)
                continue;

            // Verify that it overrides the correct owner
            if (factories[current].fac->overridesOwner != *factories[overrideElm].fac->rootOwner)
                continue;

            // Check if multiple overrides are present for same owner and name
            if (factories[current].index != static_cast<size_t>(-1))
            {
                //std::cout << "[ERROR] Two or more factories are trying to override same factory: "
                //        << factories[current].fac->name << " on path: " << path << std::endl;

                return {};
            }

            factories[overrideElm].index = current;
            break;
        }
    }


    // factories: [ptr, index]
    // 'index' is an index into the 'factories' representing which factory is it overriding
    // FindChild loop in the list using Floyd’s Cycle-Finding Algorithm
    // But holy fk this is ugly...
    for (size_t i = 0; i < factories.size(); i++)
    {
        size_t fastPtr = factories[i].index;
        size_t slowPtr = factories[i].index;

        while (fastPtr != static_cast<size_t>(-1)
               && slowPtr != static_cast<size_t>(-1)
               && factories[fastPtr].index != static_cast<size_t>(-1))
        {
            slowPtr = factories[slowPtr].index;
            fastPtr = factories[factories[fastPtr].index].index;

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
        if (fac.index == static_cast<size_t>(-1))
            continue;

        fac.fac = nullptr;
    }


    // Print some nice information
    /*std::cout << "[INFO] Survived factories: ";
    bool firstPrint = true;
    for (auto WF: factories)
    {
        if (!WF.fac)
            continue;

        if (!firstPrint)
            std::cout << ", ";

        std::cout << WF.fac->name << " (" << *(WF.fac->rootOwner) << ")";
        firstPrint = false;
    }
    std::cout << std::endl;*/


    // Build the actual objects
    std::vector<std::shared_ptr<GObj>> builtObjects;
    for (auto& facElm: factories)
    {
        if (facElm.fac && facElm.fac->ptr)
        {
            // Call facElm (pointer) aka. builder factory

            std::shared_ptr hndl = facElm.fac->ptr(facElm.fac->name, pathObj, facElm.fac);

            // Give the new obj a weak hndl to itself
            hndl->p_weakSelf = hndl;

            // Send Init event
            builtObjects.push_back(hndl);
            facElm.fac->built = true;
        }
    }

    return builtObjects;
}

bool ObjectFactories::CreateOwner(std::string_view owner)
{
    if (owner.empty())
    {
        std::cout << "[ERROR] 'owner' cannot be empty an string when creating new owner" << std::endl;
    }

    std::lock_guard lck(m_accessMutex);

    size_t ownerIndex = 0;
    for (; ownerIndex < m_facRoots.size(); ownerIndex++)
    {
        if (m_facRoots[ownerIndex]->ownerName == owner)
            break;
    }

    if (ownerIndex != m_facRoots.size())
    {
        std::cout << "[ERROR] Owner with name \"" << owner << "\" already exists" << std::endl;
        return false;
    }

    if (m_defaultOwnerName.empty())
        m_defaultOwnerName = owner;

    //std::cout << "[DEBUG] Creating owner: " << owner << std::endl;

    m_facRoots.push_back(new FactoryRoot(owner));
    return true;
}
