#pragma once

#include "BaseWindow.h"
#include "Quad.h"


class SimpleChild2 : public Galvanizer::BaseWindow
{
public:
    static std::shared_ptr<Galvanizer::GObj> factory(std::string_view name,
                                                     const std::weak_ptr<Galvanizer::GObj>& parent,
                                                     Galvanizer::Factory* originFac);
    ~SimpleChild2() override;

    uintptr_t Callback(const std::shared_ptr<Galvanizer::Event>& event) override;

protected:
    SimpleChild2(std::string_view name, const std::weak_ptr<Galvanizer::GObj>& parent, Galvanizer::Factory* originFac);

private:
    Galvanizer::Quad m_bkg;
};
