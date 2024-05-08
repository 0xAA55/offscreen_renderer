#include "rtask.hpp"
#include "tasksolver.hpp"

namespace RenderTaskSolver
{
    FBOException::FBOException(std::string what) noexcept :
        std::runtime_error(what)
    {
    }

    RenderTask::RenderTask(TaskSolver& Solver, std::shared_ptr<TaskShader> Shader, std::vector<std::string> Inputs, std::vector<std::string> Outputs, std::vector<std::string> ShaderStorages) :
        gl(Solver.gl),
        Solver(Solver),
        Shader(Shader),
        Inputs(Inputs), Outputs(Outputs), ShaderStorages(ShaderStorages)
    {
    }

    RenderTaskDraw::RenderTaskDraw(TaskSolver& Solver, std::shared_ptr<TaskShader> Shader, std::vector<std::string> Inputs, std::vector<std::string> Outputs, std::vector<std::string> ShaderStorages) :
        RenderTask(Solver, Shader, Inputs, Outputs, ShaderStorages),
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

    void RenderTaskDraw::InitFBO()
    {
        if (FBO) return;

        gl.GenFramebuffers(1, &FBO);
        gl.BindFramebuffer(gl.FRAMEBUFFER, FBO);

        GLenum attachment = gl.COLOR_ATTACHMENT0;
        for (auto& o : Outputs)
        {
            auto& t = Solver.TextureMap[o];
            OutWidth = max(OutWidth, t->GetWidth());
            OutHeight = max(OutHeight, t->GetHeight());
            gl.FramebufferTexture2D(gl.FRAMEBUFFER, attachment, gl.TEXTURE_2D, *t, 0);
            attachment++;
        }

        gl.BindFramebuffer(gl.FRAMEBUFFER, 0);
    }

    void RenderTaskDraw::OnLoadingFinished()
    {
        InitFBO();
    }

    void RenderTaskDraw::Process()
    {
        GLuint Program = *Shader;

        gl.BindFramebuffer(gl.FRAMEBUFFER, FBO);
        gl.Viewport(0, 0, OutWidth, OutHeight);

        gl.UseProgram(Program);
        gl.BindVertexArray(Pipe);

        GLenum TexUnit = 0;
        GLint location = -1;

        for (auto& i : Inputs)
        {
            if (!i.size()) continue;
            location = gl.GetUniformLocation(Program, i.c_str());
            if (location == -1) continue;

            auto& texture = *Solver.TextureMap[i];
            gl.ActiveTexture(gl.TEXTURE0 + TexUnit);
            gl.BindTexture(gl.TEXTURE_2D, texture);
            gl.Uniform1i(location, TexUnit);
            TexUnit++;
        }

        location = gl.GetUniformLocation(Program, "Resolution");
        if (location != -1) gl.Uniform2f(location, static_cast<GLfloat>(OutWidth), static_cast<GLfloat>(OutHeight));

        BufferBind<gl.ELEMENT_ARRAY_BUFFER> b(gl, Solver.BBEB);
        gl.DrawElements(gl.TRIANGLES, 6, gl.UNSIGNED_BYTE, nullptr);
        gl.BindFramebuffer(gl.FRAMEBUFFER, 0);
    }

    RenderTaskCompute::RenderTaskCompute(TaskSolver& Solver, std::shared_ptr<TaskShader> Shader, std::vector<std::string> Inputs, std::vector<std::string> Outputs, std::vector<std::string> ShaderStorages, uint32_t numGroupX, uint32_t numGroupY, uint32_t numGroupZ) :
        RenderTask(Solver, Shader, Inputs, Outputs, ShaderStorages),
        numGroupX(numGroupX), numGroupY(numGroupY), numGroupZ(numGroupZ)
    {
    }

    void RenderTaskCompute::OnLoadingFinished()
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

            gl.BindImageTexture(index, texture, 0, gl.FALSE, 0, gl.READ_ONLY, TexFormatToComputeFormat.at(texture.GetFormat()));
            index++;
        }

        for (auto& out : Outputs)
        {
            auto& texture = *Solver.TextureMap[out];

            gl.BindImageTexture(index, texture, 0, gl.FALSE, 0, gl.READ_WRITE, TexFormatToComputeFormat.at(texture.GetFormat()));
            index++;
        }

        index = 0;
        for (auto& ss : ShaderStorages)
        {
            auto& storage = *Solver.ShaderStorageMap[ss];

            gl.BindBufferBase(gl.SHADER_STORAGE_BUFFER, index, storage);
            index++;
        }

        gl.DispatchCompute(numGroupX, numGroupY, numGroupZ);
        gl.MemoryBarrier(gl.SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
}
