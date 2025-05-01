#include <iostream>

#include "MainWindow.h"
#include "Application.h"
#include "EventConfigurations.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "shaders/Default.inl"

using namespace Galvanizer;
using namespace EventConfiguration;


namespace
{
struct MainWindow_Shared : MainWindow
{
    template<typename... Args>
    MainWindow_Shared(Args&&... args): MainWindow(std::forward<Args>(args)...) {}
};
}

std::shared_ptr<GObj> MainWindow::factory(std::string_view name, const std::weak_ptr<GObj>& parent, Factory* originFac)
{
    return std::make_shared<MainWindow_Shared>(name, parent, originFac);
}

MainWindow::MainWindow(const std::string_view name, const std::weak_ptr<GObj>& parent, Factory* originFac)
    : BaseWindow(name, parent, originFac), p_eventLoop(&m_BO, true), p_mainELRef(&p_eventLoop)
{
    // Create new ELRef
    p_eventLoopRef = &p_mainELRef;

    p_mainWindowRef = this;
    p_eventLoop.Start();

    p_vertShader = g_defaultVert;
    p_fragShader = g_defaultFrag;

    p_isMainWin = true;
}

MainWindow::~MainWindow()
{
    auto event = CreateWindowEvent<WindowMessage::DestroyWindow>(p_weakSelf);
    Application::get()->PostEvent(event, p_weakSelf);
}


uintptr_t MainWindow::Dispatcher(const std::shared_ptr<Event>& event)
{
    if (event->IsType<WindowEvent>())
    {
        auto& winEvent = static_cast<WindowEvent&>(*event);

        if (winEvent.message == WindowMessage::Render ||
            winEvent.message == WindowMessage::RenderInit ||
            winEvent.message == WindowMessage::RenderExit)
        {
            if (event->sender.lock() != p_weakSelf.lock())
                return 0;   // Event coming from another 'parent' window - ignore it
        }

        if (winEvent.message == WindowMessage::Render)
        {
            renderer.UpdateTextures();
            renderer.SetSpace(IVec2(0, 0), Utility::Round(p_pxSize));
            renderer.Clear();

            uintptr_t ret = BaseWindow::Dispatcher(event);

            glfwSwapBuffers(static_cast<GLFWwindow*>(p_winHNDL));
            return ret;
        }
    }

    else if (event->IsType<MouseEvent>())
    {
        auto& mouseEvent = static_cast<MouseEvent&>(*event);

        // Ignore the "Enter" event coming from the GLFW.
        // It is directly followed by the "Move" event which is used internally to generate "Enter" event
        if (mouseEvent.message == MouseMessage::Enter)
            return 0;

        if (mouseEvent.message == MouseMessage::Leave)
        {
            uintptr_t ret = 0;
            if (auto win = winUnderCursor.lock())
            {
                ret = win->Callback(event);
                winUnderCursor.reset();
            }

            return ret;
        }

        if (auto win = mouseFocusWin.lock())
        {
            mouseEvent.pos -= win->GetAbsPos();
            uintptr_t ret = win->Callback(event);
            mouseEvent.pos += win->GetAbsPos();

            // The focus has changed (not focused anymore)
            if (mouseFocusWin.lock() != win)
                ret = BaseWindow::Dispatcher(CreateMouseEvent<MouseMessage::Move>(mouseEvent.pos));

            return ret;
        }
    }

    else if (event->IsType<KeyEvent>())
    {
        if (auto focus = keyboardFocus.lock())
            return focus->Callback(event);
        else
            return Callback(event);
    }

    return BaseWindow::Dispatcher(event);
}


uintptr_t MainWindow::Callback(const std::shared_ptr<Event>& event)
{
    if (event->IsType<ObjectEvent>())
    {
        auto& objectEvent = static_cast<ObjectEvent&>(*event);
        switch (objectEvent.message)
        {
        case ObjectMessage::Init:
        {
            // [TODO] Fix this: CreateWindow takes in a char* for name, horrible!!!
            thread_local std::string name = GetTarget();
            auto createWin = CreateWindowEvent<WindowMessage::CreateWindow>(p_weakSelf, p_virtualSize,
                                                                            name.c_str(), p_parentMainWindow);
            Application::get()->PostEvent(createWin, p_weakSelf);
            break;
        }

        case ObjectMessage::Close:
        {
            // If we close before registering the window
            if (m_renderRunning)
            {
                m_renderRunning = false;
                m_renderSemaphore.release();
                m_renderThread.join();
            }

            if (p_winHNDL)
            {
                glfwSetWindowUserPointer(static_cast<GLFWwindow*>(p_winHNDL), nullptr);

                auto destroy = CreateWindowEvent<WindowMessage::DestroyWindow>(p_weakSelf);
                Application::get()->PostEvent(destroy, p_weakSelf);
            }

            break;
        }

        default:
            break;
        }
    }

    else if (event->IsType<WindowEvent>())
    {
        auto& winEvent = static_cast<WindowEvent&>(*event);

        switch (winEvent.message)
        {
        case WindowMessage::RegisterHNDL:
        {
            auto winHNDL = static_cast<GLFWwindow*>(winEvent.winHNDL);
            p_winHNDL = winHNDL;

            // Set *Before* starting the thread - otherwise, its race condition in waiting
            m_renderRunning = true;
            m_renderThread = std::thread(&MainWindow::RenderLoop, this);

            break;
        }

        case WindowMessage::RenderRequest:
        {
            if (m_renderRequests < 2)
                m_renderRequests++;

            m_renderSemaphore.release();
            return 0;
        }

        case WindowMessage::RenderInit:
        {
            renderer.Init(p_vertShader, p_fragShader);
            break;
        }

        case WindowMessage::RenderExit:
        {
            renderer.Exit();
            break;
        }

        case WindowMessage::Close:
        {
            p_parent.lock()->PostEvent(CreateObjectEvent<ObjectMessage::Close>(p_weakSelf), p_weakSelf);
            break;
        }

        case WindowMessage::ResizeRequest:
        {
            Application::get()->PostEvent(CreateWindowEvent<WindowMessage::Resize>(p_weakSelf, winEvent.size),
                                          p_weakSelf);
            return 0;
        }

        case WindowMessage::Scale:
        {
            m_scale = winEvent.scale;
            PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);

            break;
        }

        default:
            break;
        }
    }

    return BaseWindow::Callback(event);
}


void MainWindow::RenderLoop()
{
    glfwMakeContextCurrent(static_cast<GLFWwindow*>(p_winHNDL));

    if (!gladLoadGL())
    {
        std::cout << "[ERROR] GLAD: Failed to initialize OpenGL context" << std::endl;
        return;
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_SCISSOR_TEST);

    auto renderInit = std::make_shared<WindowEvent>();
    renderInit->visibility = EventVisibility::Root;
    renderInit->priority = ChildPriority::Last;
    renderInit->message = WindowMessage::RenderInit;
    renderInit->sender = p_weakSelf;
    Dispatcher(renderInit);

    PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);
    PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);

    while (m_renderRunning)
    {
        m_renderSemaphore.acquire();

        if (m_renderRequests > 0)
        {
            auto render = std::make_shared<WindowEvent>();
            render->visibility = EventVisibility::Root;
            render->priority = ChildPriority::Last;
            render->message = WindowMessage::Render;
            render->sender = p_weakSelf;

            //std::cout << "[DEBUG] Started rendering" << std::endl;
            Dispatcher(render);
            //std::cout << "[DEBUG] Finished rendering" << std::endl;

            m_renderRequests--;
        }
    }


    auto renderExit = std::make_shared<WindowEvent>();
    renderExit->visibility = EventVisibility::Root;
    renderExit->priority = ChildPriority::Last;
    renderExit->message = WindowMessage::RenderExit;
    renderExit->sender = p_weakSelf;
    Dispatcher(renderExit);

    glfwSetWindowUserPointer(static_cast<GLFWwindow*>(p_winHNDL), nullptr);
    glfwMakeContextCurrent(nullptr);
}
