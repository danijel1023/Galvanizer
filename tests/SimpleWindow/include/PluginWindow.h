#pragma once

#include "BaseWindow.h"


class PluginWindow : public Galvanizer::BaseWindow
{
public:
    static Galvanizer::GObjHNDL factory(std::string_view name, Galvanizer::GObjHNDL parent);

    ~PluginWindow() override;

    uintptr_t Callback(std::shared_ptr<Galvanizer::Event> event) override;

protected:
    PluginWindow(std::string_view name, Galvanizer::GObjHNDL parent);
};
