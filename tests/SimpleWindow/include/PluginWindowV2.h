#pragma once

#include "BaseWindow.h"


class PluginWindowV2 : public Galvanizer::BaseWindow
{
public:
    static Galvanizer::GObjHNDL factory(std::string_view name, const Galvanizer::WeakRef &parent,
                                        Galvanizer::Factory *originFac);
    ~PluginWindowV2() override;
    uintptr_t Callback(const std::shared_ptr<Galvanizer::Event> &event) override;

protected:
    PluginWindowV2(std::string_view name, const Galvanizer::WeakRef &parent, Galvanizer::Factory *originFac);
};
