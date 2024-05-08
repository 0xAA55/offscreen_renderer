#include "rtask.hpp"
#include "tasksolver.hpp"
#include "strutil.hpp"

#include <iostream>
#include <memory>

namespace RenderTaskSolver
{
    using namespace StringUtil;

    FBOException::FBOException(const std::string& what) noexcept :
        std::runtime_error(what)
    {
    }

    std::set<std::string> FilterStringWithLength(const std::set<std::string>& input)
    {
        std::set<std::string> ret;
        for (auto i : input)
        {
            TrimInPlace(i);
            if (i.size()) ret.insert(i);
        }
        return ret;
    }

    RenderTask::RenderTask(TaskSolver& Solver, const std::string& Name, std::shared_ptr<TaskShader> Shader, std::set<std::string> Inputs, std::set<std::string> Outputs, std::set<std::string> ShaderStorages) :
        gl(Solver.gl),
        Name(Name),
        Solver(Solver),
        Shader(Shader),
        Inputs(FilterStringWithLength(Inputs)), Outputs(FilterStringWithLength(Outputs)), ShaderStorages(FilterStringWithLength(ShaderStorages))
    {
        InputsAndOutputs = Inputs;
        InputsAndOutputs.merge(Outputs);
    }

    RenderTaskDraw::RenderTaskDraw(TaskSolver& Solver, const std::string& Name, std::shared_ptr<TaskShader> Shader, std::set<std::string> Inputs, std::set<std::string> Outputs, std::set<std::string> ShaderStorages) :
        RenderTask(Solver, Name, Shader, Inputs, Outputs, ShaderStorages),
        FBO(0), OutWidth(0), OutHeight(0)
    {
        GLuint Program = *Shader;
        gl.UseProgram(Program);

        gl.GenVertexArrays(1, &Pipe);
        gl.BindVertexArray(Pipe);

        GLint location = gl.GetAttribLocation(Program, "Position");
        if (location != -1)
        {
            BufferBind<gl.ARRAY_BUFFER> b(gl, Solver.BBVB);
            gl.EnableVertexAttribArray(location);
            gl.VertexAttribPointer(location, 2, gl.FLOAT, gl.FALSE, 8, 0);
        }

        gl.BindVertexArray(0);
    }

    RenderTaskDraw::~RenderTaskDraw()
    {
        gl.DeleteVertexArrays(1, &Pipe);
        gl.DeleteFramebuffers(1, &FBO);
    }

    template<typename T> T max(const T v1, const T v2)
    {
        return v1 < v2 ? v2 : v1;
    }

    void RenderTaskDraw::EnsureFBO(GLuint Program)
    {
        if (FBO) return;

        gl.GenFramebuffers(1, &FBO);
        gl.BindFramebuffer(gl.DRAW_FRAMEBUFFER, FBO);

        GLint attachment = 0;
        auto draw_buffers = std::make_unique<GLenum[]>(Outputs.size());
        for (auto& o : Outputs)
        {
            auto& t = Solver.TextureMap[o];

            GLint location = gl.GetFragDataLocation(Program, t->ShaderOutputName.c_str());
            if (location >= 0)
            {
                if (Solver.Verbose)
                {
                    std::cout << "Binding `" << t->GetName() << "` to framebuffer attachment " << location << "." << std::endl;
                }
                OutWidth = max(OutWidth, t->GetWidth());
                OutHeight = max(OutHeight, t->GetHeight());
                gl.FramebufferTexture2D(gl.DRAW_FRAMEBUFFER, gl.COLOR_ATTACHMENT0 + location, gl.TEXTURE_2D, *t, 0);
            }
            else
            {
                if (Solver.Verbose)
                {
                    std::cout << "Could not bind `" << t->GetName() << "` to a framebuffer attachment " << location << "." << std::endl;
                }
            }
            draw_buffers[attachment] = gl.COLOR_ATTACHMENT0 + location;
            attachment++;
        }

        gl.DrawBuffers(Outputs.size(), &draw_buffers[0]);
        gl.BindFramebuffer(gl.DRAW_FRAMEBUFFER, 0);
    }

    void RenderTaskDraw::Process()
    {
        GLuint Program = *Shader;

        EnsureFBO(Program);
        gl.BindFramebuffer(gl.DRAW_FRAMEBUFFER, FBO);
        gl.Viewport(0, 0, OutWidth, OutHeight);

        if (Solver.Verbose)
        {
            std::cout << "Viewport size is " << OutWidth << "x" << OutHeight << "." << std::endl;
        }

        gl.UseProgram(Program);
        gl.BindVertexArray(Pipe);

        GLenum TexUnit = 0;
        GLuint ssbo_binding = 0;
        GLint location = -1;

        for (auto& i : Inputs)
        {
            auto& texture = *Solver.TextureMap[i];
            location = gl.GetUniformLocation(Program, texture.UniformName.c_str());
            if (location == -1)
            {
                if (Solver.Verbose)
                {
                    std::cout << "Warning: could not find location of uniform `" << texture.UniformName << "` of shader `" << Shader->GetShaderName() << "`" << std::endl;
                }
                continue;
            }

            gl.ActiveTexture(gl.TEXTURE0 + TexUnit);
            gl.BindTexture(gl.TEXTURE_2D, texture);
            gl.Uniform1i(location, TexUnit);
            TexUnit++;
        }

        location = gl.GetUniformLocation(Program, "Resolution");
        if (location != -1) gl.Uniform2f(location, static_cast<GLfloat>(OutWidth), static_cast<GLfloat>(OutHeight));

        ssbo_binding = 0;
        for (auto& ss : ShaderStorages)
        {
            auto& storage = *Solver.ShaderStorageMap[ss];

            gl.BindBufferBase(gl.SHADER_STORAGE_BUFFER, ssbo_binding, storage);
            ssbo_binding++;
        }

        BufferBind<gl.ELEMENT_ARRAY_BUFFER> b(gl, Solver.BBEB);
        gl.DrawElements(gl.TRIANGLES, 6, gl.UNSIGNED_BYTE, nullptr);
        gl.BindFramebuffer(gl.DRAW_FRAMEBUFFER, 0);
    }

    RenderTaskCompute::RenderTaskCompute(TaskSolver& Solver, const std::string& Name, std::shared_ptr<TaskShader> Shader, std::set<std::string> Inputs, std::set<std::string> Outputs, std::set<std::string> ShaderStorages, uint32_t numGroupX, uint32_t numGroupY, uint32_t numGroupZ) :
        RenderTask(Solver, Name, Shader, Inputs, Outputs, ShaderStorages),
        numGroupX(numGroupX), numGroupY(numGroupY), numGroupZ(numGroupZ)
    {
    }

    void RenderTaskCompute::Process()
    {
        GLuint index = 0;
        static const std::unordered_map<TextureFormat, GLenum> TexFormatToComputeFormat =
        {
            {TextureFormat::RGBA8, GL::Version11::RGBA8},
            {TextureFormat::RGBA16, GL::Version11::RGBA16},
            {TextureFormat::RGBA32I, GL::Version30::RGBA32I},
            {TextureFormat::RGBA32F, GL::Version30::RGBA32F},
        };

        GLuint Program = *Shader;
        gl.UseProgram(Program);

        for (auto& in : Inputs)
        {
            auto& texture = *Solver.TextureMap[in];

            if (Solver.Verbose)
            {
                std::cout << "Binding image `" << texture.UniformName << "` to " << index << "." << std::endl;
            }
            gl.BindImageTexture(index, texture, 0, gl.FALSE, 0, gl.READ_ONLY, TexFormatToComputeFormat.at(texture.GetFormat()));
            index++;
        }

        for (auto& out : Outputs)
        {
            auto& texture = *Solver.TextureMap[out];

            if (Solver.Verbose)
            {
                std::cout << "Binding image `" << texture.UniformName << "` to " << index << "." << std::endl;
            }
            gl.BindImageTexture(index, texture, 0, gl.FALSE, 0, gl.READ_WRITE, TexFormatToComputeFormat.at(texture.GetFormat()));
            index++;
        }

        index = 0;
        for (auto& ss : ShaderStorages)
        {
            auto& storage = *Solver.ShaderStorageMap[ss];

            if (Solver.Verbose)
            {
                std::cout << "Binding shader storage `" << storage.GetName() << "` to " << index << "." << std::endl;
            }
            gl.BindBufferBase(gl.SHADER_STORAGE_BUFFER, index, storage);
            index++;
        }

        gl.DispatchCompute(numGroupX, numGroupY, numGroupZ);
        gl.MemoryBarrier(gl.SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
}
