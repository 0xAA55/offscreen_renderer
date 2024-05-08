#include <iostream>

#include "glfwctx.hpp"
#include "tasksolver.hpp"

using namespace RenderTaskSolver;

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        std::cout << "Usage: " << argv[0] << "<config file> [options]" << std::endl;
        return 1;
    }

    std::set<std::string> Options;
    for (int i = 2; i < argc; i++) Options.insert(argv[i]);

    HiddenGLFWWindow w;
    try
    {
        TaskSolver s(w.GetOpenGLContext(), argv[1], Options);
        s.SolveTasks();
    }
    catch (const std::runtime_error& e)
    {
        std::cout << e.what();
    }
    
    return 0;
}
