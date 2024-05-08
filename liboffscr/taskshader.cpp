#include <fstream>
#include <sstream>
#include "taskshader.hpp"

namespace RenderTaskSolver
{
    ShaderCompileError::ShaderCompileError(const std::string& what) noexcept :
        std::runtime_error(what)
    {
    }

    ShaderLinkageError::ShaderLinkageError(const std::string& what) noexcept :
        std::runtime_error(what)
    {
    }

    TaskShader::TaskShader(const Context& gl, const std::string& ShaderName) :
        gl(gl),
        ShaderName(ShaderName),
        ShaderProgram(gl.CreateProgram())
    {
    }

    std::string TaskShader::GetProgramInfoLog()
    {
        GLint iv;
        GLsizei si, so;
        gl.GetProgramiv(ShaderProgram, gl.INFO_LOG_LENGTH, &iv);
        si = iv;
        auto buf = std::make_unique<char[]>(si);
        gl.GetProgramInfoLog(ShaderProgram, si, &so, &buf[0]);
        return std::string(buf.get());
    }

    std::string TaskShader::LoadShaderCodeFromFile(const std::string& FilePath)
    {
        std::ifstream ifs(FilePath);
        if (ifs.fail())
        {
            std::stringstream sserr;
            sserr << "Could not open `" << FilePath << "`.";
            throw ShaderCompileError(sserr.str());
        }
        std::stringstream ss;
        ss << ifs.rdbuf();
        return ss.str();
    }

    std::string TaskShader::GetShaderInfoLog(GLuint Shader)
    {
        GLint iv;
        GLsizei si, so;
        gl.GetShaderiv(Shader, gl.INFO_LOG_LENGTH, &iv);
        si = iv;
        auto buf = std::make_unique<char[]>(si);
        gl.GetShaderInfoLog(Shader, si, &so, &buf[0]);
        return std::string(buf.get());
    }

    void TaskShader::AttachShader(GLenum ShaderType, const std::string& ShaderCode)
    {
        GLuint Shader = gl.CreateShader(ShaderType);
        gl.AttachShader(ShaderProgram, Shader);
        gl.DeleteShader(Shader);
        const char* codes[] = { ShaderCode.c_str() };
        const GLint sizes[] = { static_cast<GL::GLint>(ShaderCode.size()) };
        gl.ShaderSource(Shader, 1, codes, sizes);
        gl.CompileShader(Shader);

        GLint iv;
        gl.GetShaderiv(Shader, gl.COMPILE_STATUS, &iv);
        if (!iv) throw ShaderCompileError(GetShaderInfoLog(Shader));
    }

    TaskShaderDraw::TaskShaderDraw(const Context& gl, const std::string& ShaderName, const std::string& FragmentShader, bool ArgIsShaderFilePath) :
        TaskShader(gl, ShaderName)
    {
        AttachShader(gl.VERTEX_SHADER, DefaultVertexShaderCode);

        std::string FragmentShaderCode;
        if (ArgIsShaderFilePath)
        {
            FragmentShaderCode = LoadShaderCodeFromFile(FragmentShader);
        }
        else
        {
            FragmentShaderCode = FragmentShader;
        }

        AttachShader(gl.FRAGMENT_SHADER, FragmentShaderCode);

        gl.LinkProgram(ShaderProgram);

        GLint iv;
        gl.GetProgramiv(ShaderProgram, gl.LINK_STATUS, &iv);
        if (!iv) throw ShaderLinkageError(GetProgramInfoLog());
    }

    TaskShader::~TaskShader()
    {
        gl.DeleteProgram(ShaderProgram);
    }

    TaskShaderCompute::TaskShaderCompute(const Context& gl, const std::string& ShaderName, const std::string& ComputeShader, bool ArgIsShaderFilePath) :
        TaskShader(gl, ShaderName)
    {
        std::string ComputeShaderCode;
        if (ArgIsShaderFilePath)
        {
            ComputeShaderCode = LoadShaderCodeFromFile(ComputeShader);
        }
        else
        {
            ComputeShaderCode = ComputeShader;
        }

        AttachShader(gl.COMPUTE_SHADER, ComputeShaderCode);

        gl.LinkProgram(ShaderProgram);

        GLint iv;
        gl.GetProgramiv(ShaderProgram, gl.LINK_STATUS, &iv);
        if (!iv) throw ShaderLinkageError(GetProgramInfoLog());
    }
}
