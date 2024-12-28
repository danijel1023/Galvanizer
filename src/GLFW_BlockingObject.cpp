#include "GLFW_BlockingObject.h"

namespace Galvanizer
{
static std::binary_semaphore g_semaphore = std::binary_semaphore(0);


void GLFW_BlockingObject::WaitForEvent()
{
    g_semaphore.acquire();
}

void GLFW_BlockingObject::Notify()
{
    g_semaphore.release();
}
}
