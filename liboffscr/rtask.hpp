#pragma once

#include <memory>
#include <string>
#include <set>
#include <stdexcept>

#include "glcontext.hpp"
#include "taskshader.hpp"

namespace RenderTaskSolver
{
    using namespace GLObjects;

    class FBOException : public std::runtime_error
    {
    public:
        FBOException(const std::string& what) noexcept;
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
        const Context& gl;
        TaskSolver& Solver;
        std::string Name;
        std::set<std::string> Inputs;
        std::set<std::string> Outputs;
        std::set<std::string> InputsAndOutputs;
        std::set<std::string> ShaderStorages;
        std::shared_ptr<TaskShader> Shader;

    public:
        RenderTask(TaskSolver& Solver, const std::string& Name, std::shared_ptr<TaskShader> Shader, std::set<std::string> Inputs, std::set<std::string> Outputs, std::set<std::string> ShaderStorages);

        inline const std::string& GetName() const { return Name; }
        inline std::set<std::string> GetInputs() const { return Inputs; }
        inline std::set<std::string> GetOutputs() const { return Outputs; }
        inline std::set<std::string> GetInputsAndOutputs() const { return InputsAndOutputs; }
        inline std::set<std::string> GetShaderStorages() const { return ShaderStorages; }

        virtual void Process() = 0;
    };

    class RenderTaskDraw : public RenderTask
    {
    protected:
        void EnsureFBO();

        GLuint FBO;
        GLuint Pipe;
        uint32_t OutWidth, OutHeight;

    public:
        RenderTaskDraw(TaskSolver& Solver, const std::string& Name, std::shared_ptr<TaskShader> Shader, std::set<std::string> Inputs, std::set<std::string> Outputs, std::set<std::string> ShaderStorages);
        ~RenderTaskDraw();

        void Process() override;
    };

    class RenderTaskCompute : public RenderTask
    {
    protected:

    public:
        RenderTaskCompute(TaskSolver& Solver, const std::string& Name, std::shared_ptr<TaskShader> Shader, std::set<std::string> Inputs, std::set<std::string> Outputs, std::set<std::string> ShaderStorages, uint32_t numGroupX, uint32_t numGroupY, uint32_t numGroupZ);

        uint32_t numGroupX;
        uint32_t numGroupY;
        uint32_t numGroupZ;

        void Process() override;
    };

}
