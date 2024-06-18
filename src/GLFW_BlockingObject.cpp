#include "GLFW_BlockingObject.h"

namespace Galvanizer
{

static std::mutex mutex;
static std::condition_variable CV;
static bool ready = false;


void GLFW_BlockingObject::WaitForEvent()
{
    std::unique_lock<std::mutex> Lck(mutex);
    CV.wait(Lck, [] { return ready; });

    ready = false;
}

void GLFW_BlockingObject::Notify()
{
    std::lock_guard<std::mutex> lg(mutex);
    ready = true;
    CV.notify_all();
}

}
