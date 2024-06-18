#include <iostream>

#include "Application.h"

#include "../include/SimpleWindow.h"


using namespace Galvanizer;

int main()
{
    Application& app = Application::get();
    //app.EnablePlugins(Application::DefaultPluginLocation);

    std::string SWFactory = "SimpleWindow";
    Factory* factory = app.GetFactory(SWFactory);

    if (!factory)
    {
        std::cout << "[ERROR] Unable to get \"" << SWFactory << "\" factory!" << std::endl;
        return -1;
    }

    factory->ptr = &SimpleWindow::factory;

    return app.Run();
}
