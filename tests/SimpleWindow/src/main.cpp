#include <iostream>

#include "Application.h"

#include "../include/SimpleWindow.h"


using namespace Galvanizer;
using namespace std::chrono;

int main()
{
    steady_clock::time_point start = steady_clock::now();

    for (int i = 0; i < 1; i++)
    {
        ObjectFactories::Init();
        Application::Init();

        std::weak_ptr app = Application::get();

        //app.EnablePlugins(Application::DefaultPluginLocation);

        std::string SWFactory = "app.SimpleWindow";
        Factory* factory = ObjectFactories::GetInstance().Get(SWFactory);

        if (!factory)
        {
            std::cout << "[ERROR] Unable to get \"" << SWFactory << "\" factory!" << std::endl;
            return -1;
        }

        factory->ptr = &SimpleWindow::factory;


        app.lock()->Run();

        ObjectFactories::Shutdown();


        std::cout << "Events:" << std::endl;
        GalvanizerObject::eventM.lock();

        for (const auto& ev: GalvanizerObject::events)
        {
            const auto& target = ev.first;
            const auto& thread = ev.second.second;
            const auto& event = *ev.second.first;

            std::cout << "On thread " << thread << " event " << event.strMessage() << " received by [" << target << "]"
                    << std::endl;
        }

        GalvanizerObject::events.clear();
        GalvanizerObject::eventM.unlock();

        std::cout << std::endl << "*=* *=* *=* *=* *=* *=* *=* *=* Shutting down iteration: " << i <<
                " *=* *=* *=* *=* *=* *=* *=* *=*" << std::endl << std::endl;
    }

    steady_clock::time_point end = steady_clock::now();

    std::cerr << "Finished execution in " << duration_cast<milliseconds>(end - start).count() << "ms" <<
            std::endl;
}
