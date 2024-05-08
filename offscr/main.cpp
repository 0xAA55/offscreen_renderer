#include <iostream>

#include <liboffscr/eglctx.hpp>
#include <liboffscr/tasksolver.hpp>

using namespace RenderTaskSolver;

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        std::cout << "Usage: " << argv[0] << " <config file> [options]" << std::endl;
        return 1;
    }

    std::set<std::string> Options;
    for (int i = 2; i < argc; i++) Options.insert(argv[i]);

    EGLCtx c;
#if _DEBUG
    if(1)
#else
    try
#endif
    {
        TaskSolver s(c.GetOpenGLContext(), argv[1], Options);
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
