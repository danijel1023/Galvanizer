#include <iostream>

#include "Application.h"
#include "SimpleWindow.h"


#include "SimpleChild.h"
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
            p_size = {854, 480};

            auto& OF = ObjectFactories::GetInstance();
            OF.Get(GetTarget() + ".org")->ptr = &SimpleChild::factory;


            /*OF.CreateOwner("plg");
            Factory* pluginFac = OF.Get(GetTarget() + ".org", "plg");
            pluginFac->ptr = &PluginWindow::factory;
            pluginFac->overridesOwner = "app";*/

            m_bkg.color = {1.0, 79.0 / 255, 64.0 / 255, 1.0};
            m_bkg.size = p_size;


            //m_texQuad, m_texCircle
            m_circle.color = {0.0 / 255, 170.0 / 255, 0.0 / 255, 1.0};
            m_circle.pos = {250, 40};
            m_circle.size = {100, 100};
            m_circle.type = QuadType::Circle;

            m_tex = std::make_shared<QuadTex>();
            m_tex->data = Utility::RenderAbstraction::LoadTexture(
                "../../../../opengl-testing.png", &m_tex->specs);
            renderer.AddTexture(m_tex);

            m_texQuad.pos = {250, 200};
            m_texQuad.size = {100, 100};
            m_texQuad.type = QuadType::Rectangle;
            m_texQuad.texSize = m_tex->specs.size;
            m_texQuad.texture = m_tex;

            break;
        }

        case ObjectMessage::Close:
        {
            MainWindow::Callback(event);

            std::cout << "====================================" << std::endl;
            std::cout << "Posting Close signal to application. thread-id: " << std::this_thread::get_id() << std::endl;

            auto app = Application::get();
            app->PostEvent(CreateObjectEvent<ObjectMessage::Close>(app));

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
        case MouseMessage::Button:
        {
            if (mouseEvent.action == MouseAction::Press && mouseEvent.button == MouseButton::L)
                PostEvent(EventConfiguration::CreateWindowEvent<WindowMessage::ResizeRequest>(IVec2(300, 300)));

            return 0;
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
            if (keyEvent.action == KeyAction::Repeat)
                break;


            if (keyEvent.button < KeyButton::LowerBound)
                std::cout << "Key: " << static_cast<char>(keyEvent.button) << std::endl;

            else if (keyEvent.button == KeyButton::Right)
            {
                if (keyEvent.action == KeyAction::Release)
                    break;

                // Start from 2 bc when the app starts up, the cursor is already 1 (Arrow)
                static auto cursorType = static_cast<CursorType>(2);
                Application::get()->PostEvent(
                    EventConfiguration::CreateAppEvent<AppMessage::SetCursor>(cursorType, p_winHNDL));

                // Abomination, but it werks
                cursorType = static_cast<CursorType>(static_cast<int>(cursorType) + 1);

                // Bound check
                if (cursorType > static_cast<CursorType>(10))
                    cursorType = static_cast<CursorType>(1);
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
    }

    else if (event->IsType<WindowEvent>())
    {
        auto winEvent = static_cast<WindowEvent&>(*event);

        switch (winEvent.message)
        {
        case WindowMessage::Render:
        {
            p_mainWindow->renderer.SetSpace(winEvent.pos + p_pos, p_size);

            renderer.AddQuad(m_bkg);
            renderer.AddQuad(m_circle);
            renderer.AddQuad(m_texQuad);
            renderer.Render();

            break;
        }

        case WindowMessage::Resize:
        {
            MainWindow::Callback(event);
            m_bkg.size = p_size;

            return 0;
        }

        default:
            break;
        }
    }

    return MainWindow::Callback(event);
}
