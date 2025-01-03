#include <iostream>

#include "Application.h"

#include "../include/SimpleWindow.h"
#include "../include/SimpleChild.h"


using namespace Galvanizer;
using namespace std::chrono;

int main()
{
    steady_clock::time_point start = steady_clock::now();

    for (int i = 0; i < 1; i++)
    {
        ObjectFactories::Init();

        Application app;
        //app.EnablePlugins(Application::DefaultPluginLocation);

        std::string SWFactory = "app.SimpleWindow";
        Factory* factory = ObjectFactories::GetInstance().Get(SWFactory);

        if (!factory)
        {
            std::cout << "[ERROR] Unable to get \"" << SWFactory << "\" factory!" << std::endl;
            return -1;
        }

        factory->ptr = &SimpleWindow::factory;


        Factory* ch1Fac = ObjectFactories::GetInstance().Get("app.SimpleWindow.child-1");
        ch1Fac->ptr = &SimpleChild::factory;

        Factory* ch0Fac = ObjectFactories::GetInstance().Get("app.SimpleWindow.child-0");
        ch0Fac->ptr = &SimpleChild::factory;


        app.Run();

        ObjectFactories::Shutdown();

        std::cout << std::endl << "*=* *=* *=* *=* *=* *=* *=* *=* Shutting down iteration: " << i <<
                " *=* *=* *=* *=* *=* *=* *=* *=*" << std::endl << std::endl;

        std::cout << "Events: " << std::endl;
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
    }

    steady_clock::time_point end = steady_clock::now();

    std::cerr << "Finished execution in " << duration_cast<milliseconds>(end - start).count() << "ms" <<
            std::endl;
}
