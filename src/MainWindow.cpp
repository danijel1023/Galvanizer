#include <iostream>

#include "MainWindow.h"
#include "Application.h"
#include "EventConfigurations.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "shaders/Default.inl"

using namespace Galvanizer;
using namespace EventConfiguration;
using namespace std::chrono;
using namespace std::chrono_literals;


GObjHNDL MainWindow::factory(const std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap)
{
    return new MainWindow(name, parent, originFac, createdOnHeap);
}

MainWindow::MainWindow(const std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap)
    : BaseWindow(name, parent, originFac, createdOnHeap), p_eventLoop(&m_BO, this), p_mainELRef(&p_eventLoop)
{
    // Create new ELRef
    p_eventLoopRef = &p_mainELRef;

    p_mainWindow = this;
    p_eventLoop.Start();

    p_vertShader = g_defaultVert;
    p_fragShader = g_defaultFrag;

    p_receiveRender = false;
}

MainWindow::~MainWindow()
{
    auto event = CreateWindowEvent<WindowMessage::DestroyWindow>(p_weakSelf);
    Application::get().PostEvent(event);
}


uintptr_t MainWindow::Dispatcher(const std::shared_ptr<Event>& event)
{
    if (event->IsType<WindowEvent>() &&
        static_cast<WindowEvent&>(*event).message == WindowMessage::Render)
    {
        renderer.UpdateTextures();
        renderer.Clear();

        uintptr_t ret = BaseWindow::Dispatcher(event);

        glfwSwapBuffers(static_cast<GLFWwindow*>(p_winHNDL));
        return ret;
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
            p_size = {300, 300};

            thread_local std::string name = GetTarget();
            auto createWin = CreateWindowEvent<WindowMessage::CreateWindow>(p_weakSelf, p_size,
                                                                            name.c_str(), p_parentMainWindow);
            Application::get().PostEvent(createWin);
            break;
        }

        case ObjectMessage::Close:
        {
            m_renderRunning = false;
            m_renderSemaphore.release();
            m_renderThread.join();

            if (p_winHNDL)
            {
                glfwSetWindowUserPointer(static_cast<GLFWwindow*>(p_winHNDL), nullptr);

                auto event = CreateWindowEvent<WindowMessage::DestroyWindow>(p_weakSelf);
                Application::get().PostEvent(event);
            }

            break;
        }

        default:
            break;
        }
    }

    else if (event->IsType<WindowEvent>())
    {
        auto winEvent = static_cast<WindowEvent&>(*event);

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
            p_parent.lock()->PostEvent(CreateObjectEvent<ObjectMessage::Close>(this));
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
    renderInit->ignoreChildOnSeparateThread = true;
    Dispatcher(renderInit);

    PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::RenderRequest>());
    PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::RenderRequest>());

    while (m_renderRunning)
    {
        m_renderSemaphore.acquire();

        if (m_renderRequests > 0)
        {
            auto render = std::make_shared<WindowEvent>();
            render->visibility = EventVisibility::Root;
            render->priority = ChildPriority::Last;
            render->message = WindowMessage::Render;
            render->ignoreChildOnSeparateThread = true;

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
    renderExit->ignoreChildOnSeparateThread = true;
    Dispatcher(renderExit);

    glfwSetWindowUserPointer(static_cast<GLFWwindow*>(p_winHNDL), nullptr);
    glfwMakeContextCurrent(nullptr);
}
