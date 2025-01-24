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
}

MainWindow::~MainWindow()
{
    auto event = CreateWindowEvent<WindowMessage::DestroyWindow>(p_weakSelf);
    Application::get().PostEvent(event);
}


uintptr_t MainWindow::Dispatcher(const std::shared_ptr<Event>& event)
{
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

            glfwMakeContextCurrent(winHNDL);
            glfwSwapBuffers(winHNDL);

            if (!gladLoadGL())
            {
                std::cout << "[ERROR] GLAD: Failed to initialize OpenGL context" << std::endl;
                return -1;
            }

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_MULTISAMPLE);
            glEnable(GL_SCISSOR_TEST);

            glClearColor((25.0f / 255.0f), (40.0f / 255.0f), (90.0f / 255.0f), 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glfwSwapBuffers(winHNDL);

            glfwMakeContextCurrent(nullptr);

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

        case WindowMessage::Render:
        {
            glViewport(0, 0, p_size.x, p_size.y);
            glScissor(0, 0, p_size.x, p_size.y);

            glClear(GL_COLOR_BUFFER_BIT);

            glfwSwapBuffers(static_cast<GLFWwindow*>(p_winHNDL));
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

    while (m_renderRunning)
    {
        m_renderSemaphore.acquire();

        if (m_renderRequests > 0)
        {
            auto render = std::make_shared<WindowEvent>();
            render->visibility = EventVisibility::Root;
            render->priority = ChildPriority::First;
            render->message = WindowMessage::Render;

            //std::cout << "[DEBUG] Started rendering" << std::endl;
            Dispatcher(render);
            //std::cout << "[DEBUG] Finished rendering" << std::endl;

            m_renderRequests--;
        }
    }

    glfwSetWindowUserPointer(static_cast<GLFWwindow*>(p_winHNDL), nullptr);
    glfwMakeContextCurrent(nullptr);
}
