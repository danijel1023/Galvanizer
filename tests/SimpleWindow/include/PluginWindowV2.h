#pragma once

#include "BaseWindow.h"


class PluginWindowV2 : public Galvanizer::BaseWindow
{
public:
    static std::shared_ptr<Galvanizer::GObj> factory(std::string_view name,
                                                     const std::weak_ptr<Galvanizer::GObj>& parent,
                                                     Galvanizer::Factory* originFac);
    ~PluginWindowV2() override;
    uintptr_t Callback(const std::shared_ptr<Galvanizer::Event>& event) override;

protected:
    PluginWindowV2(std::string_view name, const std::weak_ptr<Galvanizer::GObj>& parent,
                   Galvanizer::Factory* originFac);
};
