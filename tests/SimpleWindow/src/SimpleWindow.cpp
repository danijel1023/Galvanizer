#include <iostream>

#include "Application.h"
#include "SimpleWindow.h"


#include "SimpleChild.h"
#include "SimpleChild2.h"
#include "PluginWindow.h"
#include "EventConfigurations.h"


using namespace Galvanizer;
using namespace EventConfiguration;

namespace
{
struct SimpleWindow_Shared : SimpleWindow
{
    template<typename... Args>
    SimpleWindow_Shared(Args&&... args): SimpleWindow(std::forward<Args>(args)...) {}
};
}

std::shared_ptr<GObj> SimpleWindow::factory(const std::string_view name, const std::weak_ptr<GObj>& parent,
                                            Factory* originFac)
{
    return std::make_shared<SimpleWindow_Shared>(name, parent, originFac);
}

SimpleWindow::SimpleWindow(const std::string_view name, const std::weak_ptr<GObj>& parent, Factory* originFac)
    : MainWindow(name, parent, originFac) {}

SimpleWindow::~SimpleWindow() = default;

uintptr_t SimpleWindow::Dispatcher(const std::shared_ptr<Event>& event)
{
    return MainWindow::Dispatcher(event);
}

uintptr_t SimpleWindow::Callback(const std::shared_ptr<Event>& event)
{
    if (event->IsType<ObjectEvent>())
    {
        auto objEvent = static_cast<ObjectEvent&>(*event);

        switch (objEvent.message)
        {
        case ObjectMessage::Init:
        {
            //p_size = {854, 480};
            p_size = {300, 300};

            auto& OF = ObjectFactories::GetInstance();
            OF.Get(GetTarget() + ".org")->ptr = &SimpleChild::factory;
            OF.Get(GetTarget() + ".org.child2")->ptr = &SimpleChild2::factory;


            /*OF.CreateOwner("plg");
            Factory* pluginFac = OF.Get(GetTarget() + ".org2", "plg");
            pluginFac->ptr = &PluginWindow::factory;
            pluginFac->overridesOwner = "app";*/

            m_bkg.color = {111.0f / 255, 62.0f / 255, 119.0f / 255, 1.0};
            m_bkg.size = p_size;

            Vec4 quadColor = {118.0f / 255, 117.0f / 255, 137.0f / 255, 1.0};
            m_q0.color = quadColor;
            m_q1.color = quadColor;
            m_q2.color = quadColor;
            m_q3.color = quadColor;

            m_q0.pos = {0, 0};
            m_q1.pos = {200, 0};
            m_q2.pos = {200, 200};
            m_q3.pos = {0, 200};

            m_q0.size = {100, 100};
            m_q1.size = {100, 100};
            m_q2.size = {100, 100};
            m_q3.size = {100, 100};

            m_tex = std::make_shared<QuadTex>();
            m_tex->data = Utility::RenderAbstraction::LoadTexture("../../../../opengl-testing.png", &m_tex->specs);
            m_q0.texture = m_tex;
            m_q0.texPos = {0, 0};
            m_q0.texSize = m_tex->specs.size;

            renderer.AddTexture(m_tex);

            break;
        }

        case ObjectMessage::Close:
        {
            MainWindow::Callback(event);

            std::cout << "====================================" << std::endl;
            std::cout << "Posting Close signal to application. thread-id: " << std::this_thread::get_id() << std::endl;

            auto app = Application::get();
            app->PostEvent(CreateObjectEvent<ObjectMessage::Close>(app), p_weakSelf);

            return 0;
        }

        default:
            break;
        }
    }

    else if (event->IsType<MouseEvent>())
    {
        auto mouseEvent = static_cast<MouseEvent&>(*event);

        switch (mouseEvent.message)
        {
        case MouseMessage::Move:
        {
            //std::cout << "[DEBUG] Parent mouse move: [" << mouseEvent.pos.x << ", " << mouseEvent.pos.y << "]" <<
            //        std::endl;
            break;
        }

        case MouseMessage::Button:
        {
            if (mouseEvent.action == MouseAction::Press && mouseEvent.button == MouseButton::L)
                PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::ResizeRequest>(Vec2(300, 300)),
                          p_weakSelf);

            std::cout << "[DEBUG] Parent press! [" << mouseEvent.pos.x << ", " << mouseEvent.pos.y << "]" <<
                    std::endl;
            break;
        }

        default:
            break;
        }
    }

    else if (event->IsType<KeyEvent>())
    {
        auto keyEvent = static_cast<KeyEvent&>(*event);

        switch (keyEvent.message)
        {
        case KeyMessage::Key:
        {
            if (keyEvent.button < KeyButton::LowerBound)
                std::cout << "Key: " << static_cast<char>(keyEvent.button) << std::endl;

            else if (keyEvent.button == KeyButton::Right)
            {
                if (keyEvent.action == KeyAction::Release || keyEvent.action == KeyAction::Repeat)
                    break;

                // Start from 2 bc when the app starts up, the cursor is already 1 (Arrow)
                static auto cursorType = static_cast<CursorType>(2);
                Application::get()->PostEvent(
                    EventConfiguration::CreateAppEvent<AppMessage::SetCursor>(cursorType, p_winHNDL), p_weakSelf);

                // Abomination, but it werks
                cursorType = static_cast<CursorType>(static_cast<int>(cursorType) + 1);

                // Bound check
                if (cursorType > static_cast<CursorType>(10))
                    cursorType = static_cast<CursorType>(1);
            }

            else if (keyEvent.button == KeyButton::Up)
                PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);

            break;
        }

        case KeyMessage::Codepoint:
        {
            std::cout << "Codepoint: " << static_cast<char>(keyEvent.codepoint) << std::endl;
            break;
        }

        default:
            break;
        }

        return 0;
    }

    else if (event->IsType<WindowEvent>())
    {
        auto winEvent = static_cast<WindowEvent&>(*event);

        switch (winEvent.message)
        {
        case WindowMessage::Render:
        {
            p_mainWindowRef->renderer.SetSpace(winEvent.pos, p_size);

            //std::cout << "[DEBUG - render]: m_bkg = [" << m_bkg.size.x << ", " << m_bkg.size.y << "]" << std::endl;

            renderer.AddQuad(m_bkg);
            renderer.AddQuad(m_q0);
            renderer.AddQuad(m_q1);
            renderer.AddQuad(m_q2);
            renderer.AddQuad(m_q3);
            renderer.Render();

            break;
        }

        case WindowMessage::Resize:
        {
            MainWindow::Callback(event);
            m_bkg.size = p_size;

            return 0;
        }


        case WindowMessage::Scale:
        {
            MainWindow::Callback(event);
            //std::cout << "[DEBUG] window scale: [" << winEvent.scale.x << ", " << winEvent.scale.y << "]" << std::endl;

            return 0;
        }

        default:
            break;
        }
    }

    return MainWindow::Callback(event);
}
