#pragma once

#include <string_view>

#include "Events.h"
#include "EventLoop.h"

namespace Galvanizer
{

class GalvanizerObject;

using GObjHNDL = GalvanizerObject*;


class GalvanizerObject
{
public:
    static GObjHNDL factory(std::string_view name, GObjHNDL parent);
    virtual ~GalvanizerObject();

    virtual uintptr_t Dispatcher(std::shared_ptr<Event> event);
    virtual uintptr_t Callback(std::shared_ptr<Event> event);

    [[nodiscard]] inline std::string_view getName() const
    { return p_name; }

    [[nodiscard]] inline GObjHNDL getParent() const
    { return p_parent; }

    [[nodiscard]] std::string getPath() const;
    [[nodiscard]] std::string getTarget() const;
    GObjHNDL FindChild(std::string_view name);

    void PostEvent(std::shared_ptr<Event> event);

protected:
    GalvanizerObject(std::string_view name, GObjHNDL parent);

protected:
    GObjHNDL p_parent = nullptr;
    std::vector<GObjHNDL> p_children;
    std::string p_name;

    EventLoop* p_eventLoopRef = nullptr;


private:
    // Needed for Application::Build() function to add child elements and
    // Application::Destruct() function to remove child elements
    friend class Application;
};

}
