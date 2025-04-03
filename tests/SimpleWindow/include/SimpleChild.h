#pragma once

#include "BaseWindow.h"
#include "Quad.h"


class SimpleChild : public Galvanizer::BaseWindow
{
public:
    static Galvanizer::GObjHNDL factory(std::string_view name, const Galvanizer::WeakRef& parent,
                                        Galvanizer::Factory* originFac, bool createdOnHeap);
    ~SimpleChild() override;

    uintptr_t Callback(const std::shared_ptr<Galvanizer::Event>& event) override;

protected:
    SimpleChild(std::string_view name, const Galvanizer::WeakRef& parent, Galvanizer::Factory* originFac,
                bool createdOnHeap);

private:
    Galvanizer::Quad m_bkg;
};
