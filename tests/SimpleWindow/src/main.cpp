#include <iostream>

#include "Application.h"

#include "../include/SimpleWindow.h"
#include "../include/SimpleChild.h"


using namespace Galvanizer;


int main()
{
    ObjectFactories::Init();

    int returnVal = 0;
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

    Factory* chFac = ObjectFactories::GetInstance().Get("app.SimpleWindow.child-0");
    chFac->ptr = &SimpleChild::factory;

    returnVal = app.Run();

    ObjectFactories::Shutdown();
    return returnVal;
}
