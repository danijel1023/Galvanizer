#include <iostream>
#include <cmath>

#include "Application.h"
#include "SimpleWindow.h"


#include "SimpleChild.h"
#include "SimpleChild2.h"
#include "PluginWindow.h"
#include "EventConfigurations.h"

#include "../vendor/glad/include/glad/glad.h"


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
        auto& objEvent = static_cast<ObjectEvent&>(*event);

        switch (objEvent.message)
        {
        case ObjectMessage::Init:
        {
            //Callback(EventConfiguration::CreateWindowEvent<WindowMessage::Scale>(Vec2(1.5, 1.5)));
            //p_size = {854, 480};
            SetVirtualSize({300, 300});

            auto& OF = ObjectFactories::GetInstance();
            OF.Get(GetTarget() + ".org")->ptr = &SimpleChild::factory;
            OF.Get(GetTarget() + ".org.child2")->ptr = &SimpleChild2::factory;


            /*OF.CreateOwner("plg");
            Factory* pluginFac = OF.Get(GetTarget() + ".org2", "plg");
            pluginFac->ptr = &PluginWindow::factory;
            pluginFac->overridesOwner = "app";*/

            m_bkg.color = {111.0f / 255, 62.0f / 255, 119.0f / 255, 1.0};
            m_bkg.size = p_virtualSize;

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

            enableOutline = true;
            outlineThickness = 1.9;
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
        auto& mouseEvent = static_cast<MouseEvent&>(*event);

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

        case MouseMessage::Scroll:
        {
            if (mouseEvent.scrollOffset.y < 0)
            {
                m_xAreaShrink++;
                //p_pxSize.x -= 1.0;
            }

            if (mouseEvent.scrollOffset.y > 0)
            {
                if (m_xAreaShrink > 0)
                {
                    m_xAreaShrink--;
                    //p_pxSize.x += 1.0;
                }
            }

            PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);
            break;
        }

        default:
            break;
        }
    }

    else if (event->IsType<KeyEvent>())
    {
        auto& keyEvent = static_cast<KeyEvent&>(*event);

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

            if (keyEvent.button == static_cast<KeyButton>('C'))
            {
                m_capture = true;
            }

            else if (keyEvent.button == static_cast<KeyButton>('O') && keyEvent.action == KeyAction::Press)
            {
                enableOutline = !enableOutline;
                PostEvent(CreateWindowEvent<WindowMessage::RenderRequest>(), p_weakSelf);
            }

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
        auto& winEvent = static_cast<WindowEvent&>(*event);

        switch (winEvent.message)
        {
        case WindowMessage::Render:
        {
            auto size = p_pxSize;
            size.x -= m_xAreaShrink;

            renderer.SetSpace(Utility::Round(winEvent.pos), Utility::Round(p_pxSize));

            //std::cout << "[DEBUG - render]: m_bkg = [" << m_bkg.size.x << ", " << m_bkg.size.y << "]" << std::endl;

            Vec2 scale = GetScale();
            Quad scaledBkg = m_bkg, scaledQ0 = m_q0, scaledQ1 = m_q1, scaledQ2 = m_q2, scaledQ3 = m_q3;
            scaledBkg.size = {std::round(m_bkg.size.x * scale.x), std::round(m_bkg.size.y * scale.y)};
            scaledQ0.size = {std::round(m_q0.size.x * scale.x), std::round(m_q0.size.y * scale.y)};
            scaledQ1.size = {std::round(m_q1.size.x * scale.x), std::round(m_q1.size.y * scale.y)};
            scaledQ2.size = {std::round(m_q2.size.x * scale.x), std::round(m_q2.size.y * scale.y)};
            scaledQ3.size = {std::round(m_q3.size.x * scale.x), std::round(m_q3.size.y * scale.y)};


            scaledBkg.pos = {std::round(m_bkg.pos.x * scale.x), std::round(m_bkg.pos.y * scale.y)};
            scaledQ0.pos = {std::round(m_q0.pos.x * scale.x), std::round(m_q0.pos.y * scale.y)};
            scaledQ1.pos = {std::round(m_q1.pos.x * scale.x), std::round(m_q1.pos.y * scale.y)};
            scaledQ2.pos = {std::round(m_q2.pos.x * scale.x), std::round(m_q2.pos.y * scale.y)};
            scaledQ3.pos = {std::round(m_q3.pos.x * scale.x), std::round(m_q3.pos.y * scale.y)};

            renderer.AddQuad(scaledBkg);
            renderer.AddQuad(scaledQ0);
            renderer.AddQuad(scaledQ1);
            renderer.AddQuad(scaledQ2);
            renderer.AddQuad(scaledQ3);
            renderer.Render();


            if (m_capture)
            {
                m_capture = false;

                int width = p_virtualSize.x * GetScale().x;
                int height = p_virtualSize.y * GetScale().y;

                auto* data = new uint8_t[width * height * 4];
                glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

                std::cout << "data: " << data << std::endl;
                delete[] data;
            }

            break;
        }

        case WindowMessage::Resize:
        {
            MainWindow::Callback(event);
            m_bkg.size = p_virtualSize;
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
