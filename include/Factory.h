#pragma once

#include <vector>
#include <string>


/*
    [TODO]: Convert tree structure into a tape with indexes
*/

namespace Galvanizer
{


// [NOTE] Possibly convert to a template class
class GalvanizerObject;
using GObjHNDL = GalvanizerObject*;
using GObjFactoryPtr = GObjHNDL(*)(std::string_view name, GObjHNDL parent);


struct Factory
{
    Factory(std::string_view name, std::string* rootOwner);
    Factory(std::string_view name, Factory* parent);
    Factory() = default;
    ~Factory();


    Factory* Find(std::string_view name);

    bool active = true;
    bool built = false;

    GObjFactoryPtr ptr = nullptr;

    // If this is a factory from plugin, it can specify which factory it wants to override.
    // If two or more factories have the same name, but overridesOwner does not match any ownerName,
    // the build command will fail to build the current namespace.
    // Example:
    // 
    // name = "app.SimpleWindow"
    // overridesOwner = ""
    // 
    // name = "plg.SimpleWindow"
    // overridesOwner = ""  ->> fail state: name conflict (SimpleWindow) without override specifier
    // 
    // However, if overridesOwner is specified but the ownerName with that name doesn't exist, build command
    // wont build the object. This can occur in case dependency plugin is not loaded or the "app" is misspelled
    // eg: "aap". Optional "debug" flag can be set to emit a warning in this case.
    std::string overridesOwner;

    std::string name;
    Factory* parent = nullptr;
    std::string* rootOwner = nullptr;

    // We don't clean any factory nodes. In most cases, it's way easier to just
    // "disable" a factory and reuse it later, than deleting the object and later
    // allocating a new one. When application terminates, the underlying OS will
    // automatically reclaim the allocated memory anyway.
    std::vector<Factory*> children;
};


// Has to be heap allocated since we are taking pointers directly from this
struct FactoryRoot
{
    std::string ownerName;
    Factory factory = Factory("root", &ownerName);

    bool operator==(std::string_view testOwner) const;
};


}
