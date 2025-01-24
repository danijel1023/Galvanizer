#include "GLFW_BlockingObject.h"

#include <atomic>

#include "GLFW/glfw3.h"

using namespace Galvanizer;


void GLFW_BlockingObject::WaitForEvent()
{
    glfwWaitEvents();
}

void GLFW_BlockingObject::Notify()
{
    glfwPostEmptyEvent();
}
