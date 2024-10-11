#pragma once

#include <string_view>
#include <memory>

#include "Events.h"
#include "EventLoop.h"

namespace Galvanizer
{
class GalvanizerObject;
struct Factory;

using GObjHNDL = GalvanizerObject*;


class GalvanizerObject
{
public:
    GalvanizerObject(std::string_view name, GObjHNDL parent, Factory* originFac);
    static GObjHNDL factory(std::string_view name, GObjHNDL parent,
                            Factory* originFac);
    virtual ~GalvanizerObject();

    virtual uintptr_t Dispatcher(std::shared_ptr<Event> event);
    virtual uintptr_t Callback(std::shared_ptr<Event> event);

    [[nodiscard]] inline std::string_view GetInternalName() const
    {
        return pc_internalName;
    }

    [[nodiscard]] inline std::string_view GetDisplayName() const
    {
        return p_displayName;
    }

    [[nodiscard]] inline GObjHNDL GetParent() const
    {
        return p_parent;
    }

    [[nodiscard]] std::string GetPath() const;
    [[nodiscard]] std::string GetTarget() const;
    GObjHNDL FindChild(std::string_view name);

    void PostEvent(std::shared_ptr<Event> event);

protected:
    GObjHNDL p_parent = nullptr;
    std::vector<GObjHNDL> p_children;
    const std::string pc_internalName;
    std::string p_displayName;

    EventLoopRef* p_eventLoopRef = nullptr;

private:
    Factory* m_originFac = nullptr;
};
}
