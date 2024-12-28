#pragma once

#include <string>

#include "BaseWindow.h"
#include "MainWindow.h"


class SimpleWindow : public Galvanizer::MainWindow
{
public:
    static Galvanizer::GObjHNDL factory(std::string_view name, const Galvanizer::WeakRef& parent,
                                        Galvanizer::Factory* originFac);
    ~SimpleWindow() override;

    uintptr_t Dispatcher(const std::shared_ptr<Galvanizer::Event>& event) override;
    uintptr_t Callback(const std::shared_ptr<Galvanizer::Event>& event) override;

protected:
    SimpleWindow(std::string_view name, const Galvanizer::WeakRef& parent, Galvanizer::Factory* originFac);
};
