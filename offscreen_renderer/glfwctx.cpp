#include "glfwctx.hpp"

#include <GLFW/glfw3.h>

namespace RenderTaskSolver
{
    static const char* GetGLFWErrorString()
    {
        const char* err;
        glfwGetError(&err);
        return err;
    }

    glfwCreateWindowFailure::glfwCreateWindowFailure(std::string what) noexcept :
        std::runtime_error(what)
    {
    }

    static void* APIENTRY GLFWGetProcAddress(const char* procname)
    {
        return glfwGetProcAddress(procname);
    }

    HiddenGLFWWindow::HiddenGLFWWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        window = glfwCreateWindow(640, 480, "", NULL, NULL);
        if (!window) throw glfwCreateWindowFailure(GetGLFWErrorString());
        glfwMakeContextCurrent(window);
        glCtx = std::make_unique<Context>(GLFWGetProcAddress);
    }

    HiddenGLFWWindow::~HiddenGLFWWindow()
    {
        glCtx.release();
        glfwMakeContextCurrent(NULL);
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    Context& HiddenGLFWWindow::GetOpenGLContext()
    {
        return *glCtx;
    }
}