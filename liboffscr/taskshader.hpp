#pragma once

#include <stdexcept>
#include <memory>
#include <string>

#include "glcontext.hpp"

namespace RenderTaskSolver
{
    using namespace GLObjects;

    class ShaderCompileError : public std::runtime_error
    {
    public:
        ShaderCompileError(const std::string& what) noexcept;
    };

    class ShaderLinkageError : public std::runtime_error
    {
    public:
        ShaderLinkageError(const std::string& what) noexcept;
    };

    class TaskShader
    {
    protected:
        const Context& gl;
        GLuint ShaderProgram;
        std::string ShaderName;

        std::string GetShaderInfoLog(GLuint Shader);
        std::string LoadShaderCodeFromFile(const std::string& FilePath);
        void AttachShader(GLenum ShaderType, const std::string& ShaderCode);

    public:
        TaskShader(const Context& gl, const std::string& ShaderName);
        ~TaskShader();

        std::string GetProgramInfoLog();
        inline operator GLuint() { return ShaderProgram; }
        inline const std::string& GetShaderName() const { return ShaderName; }
    };

    class TaskShaderDraw : public TaskShader
    {
    protected:
        const std::string DefaultVertexShaderCode = "#version 300 es\n"
            "in vec2 Position;"
            "void main()"
            "{"
            "    gl_Position = vec4(Position, 0.0, 1.0);"
            "}";

    public:
        TaskShaderDraw() = delete;
        TaskShaderDraw(const Context& gl, const std::string& ShaderName, const std::string& FragmentShader, bool ArgIsShaderFilePath);
    };


    class TaskShaderCompute : public TaskShader
    {
    protected:

    public:
        TaskShaderCompute() = delete;
        TaskShaderCompute(const Context& gl, const std::string& ShaderName, const std::string& ComputeShader, bool ArgIsShaderFilePath);
    };

}
