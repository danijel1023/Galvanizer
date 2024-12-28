#pragma once

#include "Queue.h"

namespace Galvanizer
{
class GLFW_BlockingObject : public IBlockingObject
{
public:
    ~GLFW_BlockingObject() override = default;

    void WaitForEvent() override;
    void Notify() override;
};
}
