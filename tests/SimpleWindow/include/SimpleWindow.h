#pragma once

#include "BaseWindow.h"
#include "MainWindow.h"
#include "Quad.h"


class SimpleWindow : public Galvanizer::MainWindow
{
public:
    static std::shared_ptr<Galvanizer::GObj> factory(std::string_view name,
                                                     const std::weak_ptr<Galvanizer::GObj>& parent,
                                                     Galvanizer::Factory* originFac);
    ~SimpleWindow() override;

    uintptr_t Dispatcher(const std::shared_ptr<Galvanizer::Event>& event) override;
    uintptr_t Callback(const std::shared_ptr<Galvanizer::Event>& event) override;

protected:
    SimpleWindow(std::string_view name, const std::weak_ptr<Galvanizer::GObj>& parent, Galvanizer::Factory* originFac);

private:
    Galvanizer::Quad m_bkg, m_circle, m_texQuad, m_texCircle;
    std::shared_ptr<Galvanizer::QuadTex> m_tex;
};
