#pragma once
#include <stdexcept>
#include <memory>
#include "glcontext.hpp"

struct GLFWwindow;

namespace RenderTaskSolver
{
    using namespace GLObjects;

    class glfwCreateWindowFailure : public std::runtime_error
    {
    public:
        glfwCreateWindowFailure(const std::string& what) noexcept;
    };

    class HiddenGLFWWindow
    {
    protected:
        GLFWwindow* window = nullptr;
        std::unique_ptr<Context> glCtx;

    public:
        HiddenGLFWWindow();
        ~HiddenGLFWWindow();

        Context& GetOpenGLContext();
    };
}
