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
        ShaderCompileError(std::string what) noexcept;
    };

    class ShaderLinkageError : public std::runtime_error
    {
    public:
        ShaderLinkageError(std::string what) noexcept;
    };

    class TaskShader
    {
    protected:
        Context& gl;
        GLuint ShaderProgram;

        std::string GetShaderInfoLog(GLuint Shader);
        std::string LoadShaderCodeFromFile(std::string FilePath);
        void AttachShader(GLenum ShaderType, std::string ShaderCode);

    public:
        TaskShader(Context& gl);
        ~TaskShader();

        std::string GetProgramInfoLog();
        inline operator GLuint() { return ShaderProgram; }
    };

    class TaskShaderDraw : public TaskShader
    {
    protected:
        const std::string DefaultVertexShaderCode = "#version 330\n"
            "in vec2 Position;"
            "void main()"
            "{"
            "    gl_Position = vec4(Position, 0.0, 1.0);"
            "}";

    public:
        TaskShaderDraw() = delete;
        TaskShaderDraw(Context& gl, std::string FragmentShader, bool ArgIsShaderFilePath);
    };


    class TaskShaderCompute : public TaskShader
    {
    protected:

    public:
        TaskShaderCompute() = delete;
        TaskShaderCompute(Context& gl, std::string ComputeShader, bool ArgIsShaderFilePath);
    };

}
