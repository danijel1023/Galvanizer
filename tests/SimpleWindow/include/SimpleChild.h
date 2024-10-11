#pragma once

#include "BaseWindow.h"


class SimpleChild
    : public Galvanizer::BaseWindow
{
public:
    static Galvanizer::GObjHNDL factory(std::string_view name, Galvanizer::GObjHNDL parent, Galvanizer::Factory* originFac);
    ~SimpleChild() override;

    uintptr_t Callback(std::shared_ptr<Galvanizer::Event> event) override;

protected:
    SimpleChild(std::string_view name, Galvanizer::GObjHNDL parent, Galvanizer::Factory* originFac);
};
