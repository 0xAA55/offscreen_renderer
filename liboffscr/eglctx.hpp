#pragma once
#include "glcontext.hpp"

#include <stdexcept>
#include <memory>

namespace RenderTaskSolver
{
    using namespace GLObjects;

    class EGLCreateContextError : public std::runtime_error
    {
    public:
        EGLCreateContextError(const std::string& what) noexcept;
    };

    class EGLCtx
    {
        std::unique_ptr<Context> glCtx;

    public:
        EGLCtx();

        const Context& GetOpenGLContext() const;
    };

}