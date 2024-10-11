#pragma once

#include <string>

#include "BaseWindow.h"
#include "MainWindow.h"


class SimpleWindow
    : public Galvanizer::MainWindow
{
public:
    static Galvanizer::GObjHNDL factory(std::string_view name, Galvanizer::GObjHNDL parent, Galvanizer::Factory* originFac);
    ~SimpleWindow() override;

    uintptr_t Callback(std::shared_ptr<Galvanizer::Event> event) override;

protected:
    SimpleWindow(std::string_view name, Galvanizer::GObjHNDL parent, Galvanizer::Factory* originFac);

private:
    std::string m_WhoAmI = "Sussy Chungus";
};
