#pragma once
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

#include "inifile.hpp"
#include "tasktex.hpp"
#include "taskshader.hpp"
#include "tasksstorage.hpp"

namespace RenderTaskSolver
{
    using namespace GLObjects;

    class InvalidTaskConfig : public std::runtime_error
    {
    public:
        InvalidTaskConfig(std::string what) noexcept;
    };

    class RenderTask;
    class RenderTaskDraw;
    class RenderTaskCompute;

    class TaskSolver
    {
    protected:
        friend class RenderTask;
        friend class RenderTaskDraw;
        friend class RenderTaskCompute;
        Context& gl;
        IniFile Config;
        std::vector<std::shared_ptr<RenderTask>> Tasks;
        std::unordered_set<std::string> TextureSet;
        std::unordered_map<std::string, std::shared_ptr<TaskTexture>> TextureMap;
        std::unordered_set<std::string> ShaderStorageSet;
        std::unordered_map<std::string, std::shared_ptr<TaskShaderStorage>> ShaderStorageMap;

        GLuint BBVB, BBEB;
        void InitDraw();

    public:
        TaskSolver(Context& gl, const IniFile& TaskConf);
        TaskSolver(Context& gl, std::string ConfFile);
        ~TaskSolver();

        void SolveTasks();
    };
}
