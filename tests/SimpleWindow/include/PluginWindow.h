#pragma once

#include "BaseWindow.h"
#include "MainWindow.h"

class PluginWindow : public Galvanizer::MainWindow
{
public:
    static std::shared_ptr<Galvanizer::GObj> factory(std::string_view name,
                                                     const std::weak_ptr<Galvanizer::GObj>& parent,
                                                     Galvanizer::Factory* originFac);
    ~PluginWindow() override;
    uintptr_t Callback(const std::shared_ptr<Galvanizer::Event>& event) override;

protected:
    PluginWindow(std::string_view name, const std::weak_ptr<Galvanizer::GObj>& parent, Galvanizer::Factory* originFac);
};
