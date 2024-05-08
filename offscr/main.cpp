#include <iostream>

#include <liboffscr/eglctx.hpp>
#include <liboffscr/glfwctx.hpp>
#include <liboffscr/tasksolver.hpp>

using namespace RenderTaskSolver;

class GLCtxBase
{
public:
    virtual const Context& GetOpenGLContext() const = 0;
};

class GLCtxEGL : public GLCtxBase
{
    EGLCtx c;

public:
    virtual const Context& GetOpenGLContext() const override
    {
        return c.GetOpenGLContext();
    }
};

class GLCtxGLFW : public GLCtxBase
{
    HiddenGLFWWindow w;

public:
    virtual const Context& GetOpenGLContext() const override
    {
        return w.GetOpenGLContext();
    }
};

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        std::cout << "Usage: " << argv[0] << " <config file> [options]" << std::endl;
        return 1;
    }

    std::set<std::string> Options;
    for (int i = 2; i < argc; i++) Options.insert(argv[i]);

    std::unique_ptr<GLCtxBase> c = nullptr;

    if (Options.contains("-glfw")) c = std::make_unique<GLCtxGLFW>();
    else if (Options.contains("-egl")) c = std::make_unique<GLCtxEGL>();
    if (!c) c = std::make_unique<GLCtxGLFW>();

#if _DEBUG
    if(1)
#else
    try
#endif
    {
        TaskSolver s(c->GetOpenGLContext(), argv[1], Options);
        s.SolveTasks();
    }
#if _DEBUG
#else
    catch (const std::exception& e)
    {
        std::cerr << e.what();
        return 2;
    }
#endif
    
    return 0;
}
