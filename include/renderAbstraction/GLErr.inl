#pragma once
#include <iostream>
#include <string>

#include "glad/glad.h"


#define GLAssert(x) \
if (!(x)) \
BreakPoint();

#define GLCall(x) \
{ \
GLClearError();\
x;\
GLAssert(GLCheckError()); \
}

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLCheckError()
{
    if (const auto err = glGetError())
    {
        std::string strErr;
        switch (err)
        {
        case GL_INVALID_ENUM:
            strErr = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            strErr = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            strErr = "GL_INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            strErr = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            strErr = "GL_OUT_OF_MEMORY";
            break;
        case GL_STACK_UNDERFLOW:
            strErr = "GL_STACK_UNDERFLOW";
            break;
        case GL_STACK_OVERFLOW:
            strErr = "GL_STACK_OVERFLOW";
            break;

        default:
            strErr = "Unknown error";
            break;
        }

        std::cout << "[OPENGL ERROR]: " << strErr << " - num: " << err << std::endl;
        return false;
    }

    return true;
}

static void BreakPoint()
{
    std::cout << std::endl;
}
