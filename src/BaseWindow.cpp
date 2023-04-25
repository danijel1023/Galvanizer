#include "BaseWindow.h"

namespace Galvanizer
{

std::string BaseWindow::getPath()
{
    std::string path;

    if (p_parent)
    {
        // Avoid copy
        path = std::move(p_parent->getPath());
        path += '.' + p_name;
    }

    else
    {
        path = p_name;
    }

    return path;
}

}
