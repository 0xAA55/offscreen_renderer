#include <iostream>

#include "glfwctx.hpp"
#include "tasksolver.hpp"

using namespace RenderTaskSolver;

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        std::cout << "Usage: " << argv[0] << "<config file>" << std::endl;
        return 1;
    }

    HiddenGLFWWindow w;
    try
    {
        TaskSolver s(w.GetOpenGLContext(), argv[1]);
        s.SolveTasks();
    }
    catch (const std::runtime_error& e)
    {
        std::cout << e.what();
    }
    
    return 0;
}
