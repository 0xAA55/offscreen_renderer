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

    // Using a hidden GLFW window to achieve offscreen rendering.
    HiddenGLFWWindow w;
    try
    {
        TaskSolver s(w.GetOpenGLContext(), argv[1], Options);
        s.SolveTasks();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what();
        return 2;
    }
    
    return 0;
}
