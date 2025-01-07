#pragma once

#include "BaseWindow.h"
#include "MainWindow.h"

class PluginWindow : public Galvanizer::MainWindow
{
public:
    static Galvanizer::GObjHNDL factory(std::string_view name, const Galvanizer::WeakRef& parent,
                                        Galvanizer::Factory* originFac, bool createdOnHeap);
    ~PluginWindow() override;
    uintptr_t Callback(const std::shared_ptr<Galvanizer::Event>& event) override;

protected:
    PluginWindow(std::string_view name, const Galvanizer::WeakRef& parent, Galvanizer::Factory* originFac,
                 bool createdOnHeap);
};
