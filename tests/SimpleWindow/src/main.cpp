#include <iostream>

#include "Application.h"

#include "../include/SimpleWindow.h"
#include "../include/SimpleChild.h"


using namespace Galvanizer;
namespace ch = std::chrono;

int main()
{
    ch::steady_clock::time_point start = ch::steady_clock::now();

    for (int i = 0; i < 500; i++)
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
    }

    ch::steady_clock::time_point end = ch::steady_clock::now();

    std::cerr << "Finished execution in " << ch::duration_cast<ch::milliseconds>(end - start).count() << "ms" <<
            std::endl;
}
