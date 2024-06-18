#pragma once

#include <thread>

#include "BaseWindow.h"
#include "Factory.h"
#include "EventLoop.h"

namespace Galvanizer
{

class MainWindow : public BaseWindow
{
public:
    static GObjHNDL factory(std::string_view name, GObjHNDL parent);
    ~MainWindow() override;

    uintptr_t Dispatcher(std::shared_ptr<Event> event) override;
    uintptr_t Callback(std::shared_ptr<Event> event) override;

protected:
    MainWindow(std::string_view name, GObjHNDL parent);

    EventLoop p_eventLoop;

private:
    BlockingObject m_BO;
};

}
