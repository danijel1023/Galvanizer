#pragma once

#include "BaseWindow.h"
#include "Factory.h"
#include "EventLoop.h"
#include "GLFW_BlockingObject.h"
#include "QuadRenderer.h"


namespace Galvanizer
{
class MainWindow : public BaseWindow
{
public:
    static GObjHNDL factory(std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap);
    ~MainWindow() override;

    uintptr_t Dispatcher(const std::shared_ptr<Event>& event) override;
    uintptr_t Callback(const std::shared_ptr<Event>& event) override;

    QuadRenderer renderer;

protected:
    MainWindow(std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap);
    [[nodiscard]] bool OpenGLInitialised() const { return m_openGLInit; }


    EventLoop p_eventLoop;
    EventLoopRef p_mainELRef;
    EventLoopRef* p_parentELRef = nullptr;
    void* p_winHNDL = nullptr;
    void* p_parentMainWindow = nullptr;
    const char *p_vertShader = nullptr, *p_fragShader = nullptr;

private:
    BlockingObject m_BO; // I have to order them like this because of initialization order
    bool m_openGLInit = false;

    std::thread m_renderThread;
    std::atomic_bool m_renderRunning = false;
    int m_renderRequests = 0;
    std::binary_semaphore m_renderSemaphore = std::binary_semaphore(0);

private:
    void RenderLoop();
};
}
