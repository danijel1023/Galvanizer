#pragma once

#include <string_view>
#include <memory>
#include <unordered_map>

#include "Events.h"
#include "EventLoop.h"
#include "GalvanizerRef.h"
#include "Factory.h"

namespace Galvanizer
{
class GalvanizerObject;
struct Factory;

using GObjHNDL = GalvanizerObject*;


class GalvanizerObject
{
public:
    static GObjHNDL factory(std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap);
    virtual ~GalvanizerObject();

    virtual uintptr_t Dispatcher(const std::shared_ptr<Event>& event);
    virtual uintptr_t Callback(const std::shared_ptr<Event>& event);

    [[nodiscard]] std::string_view GetInternalName() const { return pc_internalName; }
    [[nodiscard]] std::string_view GetDisplayName() const { return p_displayName; }
    [[nodiscard]] WeakRef GetParent() const { return p_parent; }

    [[nodiscard]] std::string GetPath() const;
    [[nodiscard]] std::string GetTarget() const;
    WeakRef FindChild(std::string_view name);

    void PostEvent(const std::shared_ptr<Event>& event);

    [[nodiscard]] bool Closing() const { return m_closing; }

    const bool c_createdOnHeap;

    static inline std::vector<std::pair<std::string, std::pair<std::shared_ptr<Event>, std::thread::id>>> events;
    static inline std::mutex eventM;

protected:
    GalvanizerObject(std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap);

    // p_weakSelf gets set after constructor and before receiving init event
    WeakRef p_parent, p_weakSelf;
    std::vector<OwningRef> p_children;
    const std::string pc_internalName;
    std::string p_displayName;

    EventLoopRef* p_eventLoopRef = nullptr;

    bool p_receiveRender = true;

private:
    Factory* m_originFac = nullptr;
    bool m_closing = false;

    friend std::vector<OwningRef> ObjectFactories::Build(const OwningRef& pathObj);
};
}
