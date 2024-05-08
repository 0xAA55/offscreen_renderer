#pragma once

#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "glcontext.hpp"
#include "taskshader.hpp"

namespace RenderTaskSolver
{
    using namespace GLObjects;

    class FBOException : public std::runtime_error
    {
    public:
        FBOException(std::string what) noexcept;
    };

    class TaskSolver;

    enum class TaskType
    {
        Task_Draw = 0,
        Task_Compute = 1
    };

    class RenderTask
    {
    protected:
        Context& gl;
        TaskSolver& Solver;
        bool Batch;
        std::vector<std::string> Inputs;
        std::vector<std::string> Outputs;
        std::vector<std::string> ShaderStorages;
        std::shared_ptr<TaskShader> Shader;

    public:
        RenderTask(TaskSolver& Solver, std::shared_ptr<TaskShader> Shader, std::vector<std::string> Inputs, std::vector<std::string> Outputs, std::vector<std::string> ShaderStorages);

        inline std::vector<std::string> GetInputs() const { return Inputs; }
        inline std::vector<std::string> GetOutputs() const { return Outputs; }
        inline std::vector<std::string> GetShaderStorages() const { return ShaderStorages; }

        virtual void OnLoadingFinished() = 0;
        virtual void Process() = 0;
    };

    class RenderTaskDraw : public RenderTask
    {
    protected:
        void InitFBO();

        GLuint FBO;
        GLuint Pipe;
        uint32_t OutWidth, OutHeight;

    public:
        RenderTaskDraw(TaskSolver& Solver, std::shared_ptr<TaskShader> Shader, std::vector<std::string> Inputs, std::vector<std::string> Outputs, std::vector<std::string> ShaderStorages);
        ~RenderTaskDraw();

        void OnLoadingFinished() override;
        void Process() override;
    };

    class RenderTaskCompute : public RenderTask
    {
    protected:
        uint32_t numGroupX;
        uint32_t numGroupY;
        uint32_t numGroupZ;

    public:
        RenderTaskCompute(TaskSolver& Solver, std::shared_ptr<TaskShader> Shader, std::vector<std::string> Inputs, std::vector<std::string> Outputs, std::vector<std::string> ShaderStorages, uint32_t numGroupX, uint32_t numGroupY, uint32_t numGroupZ);

        void OnLoadingFinished() override;
        void Process() override;
    };

}
