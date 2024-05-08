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

    typedef void* EGLDisplay;
    typedef void* EGLSurface;
    typedef void* EGLContext;

    class EGLCtx
    {
        std::unique_ptr<EGLDisplay> EGLDisplayPtr;
        std::unique_ptr<EGLSurface> EGLSurfacePtr;
        std::unique_ptr<EGLContext> EGLContextPtr;
        std::unique_ptr<Context> glCtx;

    public:
        EGLCtx();
        ~EGLCtx();

        const Context& GetOpenGLContext() const;
    };

}