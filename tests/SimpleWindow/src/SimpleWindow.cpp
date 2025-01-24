#include <iostream>

#include "Application.h"
#include "SimpleWindow.h"


#include "SimpleChild.h"
#include "PluginWindow.h"
#include "EventConfigurations.h"


using namespace Galvanizer;
using namespace EventConfiguration;

GObjHNDL SimpleWindow::factory(const std::string_view name, const WeakRef& parent, Factory* originFac,
                               bool createdOnHeap)
{
    return new SimpleWindow(name, parent, originFac, createdOnHeap);
}

SimpleWindow::SimpleWindow(const std::string_view name, const WeakRef& parent, Factory* originFac, bool createdOnHeap)
    : MainWindow(name, parent, originFac, createdOnHeap) {}

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
            auto& OF = ObjectFactories::GetInstance();
            OF.Get(GetTarget() + ".org")->ptr = &SimpleChild::factory;


            OF.CreateOwner("plg");
            Factory* pluginFac = OF.Get(GetTarget() + ".org", "plg");
            pluginFac->ptr = &PluginWindow::factory;
            pluginFac->overridesOwner = "app";

            break;
        }

        case ObjectMessage::Close:
        {
            MainWindow::Callback(event);

            std::cout << "====================================" << std::endl;
            std::cout << "Posting Close signal to application. thread-id: " << std::this_thread::get_id() << std::endl;

            auto& app = Application::get();
            app.PostEvent(CreateObjectEvent<ObjectMessage::Close>(&app));

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
                Application::get().PostEvent(
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

    return MainWindow::Callback(event);
}
