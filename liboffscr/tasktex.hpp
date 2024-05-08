#pragma once
#include <string>
#include <stdexcept>
#include <set>
#include <thread>

#include "glcontext.hpp"
#include "taskthread.hpp"

namespace RenderTaskSolver
{
    using namespace GLObjects;

    enum class TexFileFormat
    {
        Unknown = 0,
        BMP,
        PNG,
        JPG,
        GIF,
        TGA,
        PPM,
        PIC,
        PNM,
        PSD,
        HDR
    };

    enum class TextureFormat
    {
        Unknown = 0,
        RGBA8,
        RGBA16,
        RGBA32I,
        RGBA32F
    };

    class LoadTextureError : public std::runtime_error
    {
    public:
        LoadTextureError(const std::string& what) noexcept;
    };

    class SaveTextureError : public std::runtime_error
    {
    public:
        SaveTextureError(const std::string& what) noexcept;
    };

    class TaskSolver;

    class TaskTexture
    {
    protected:
        TaskSolver& Solver;
        const Context& gl;
        std::string Name;
        GLuint glTex;
        TextureFormat TF;
        uint32_t Width;
        uint32_t Height;

        ThreadIdType SaveFileInternal(TaskThreadManager* thrdman);

    public:
        inline const std::string& GetName() const { return Name; }
        inline uint32_t GetWidth() const { return Width; }
        inline uint32_t GetHeight() const { return Height; }
        inline TextureFormat GetFormat() const { return TF; }
        inline operator GLuint() const { return glTex; }

        std::string UniformName;
        std::string ShaderOutputName;
        std::string SavePath;
        TexFileFormat SaveFormat;
        bool DontKeep;
        bool HasContent;

        TaskTexture(TaskSolver& Solver, const std::string& Name, const std::string& ShaderOutputName, const std::string& LoadFrom, TexFileFormat Format);
        TaskTexture(TaskSolver& Solver, const std::string& Name, const std::string& ShaderOutputName, uint32_t Width, uint32_t Height, TextureFormat Format);
        ~TaskTexture();

        void SaveFile();
        ThreadIdType ThreadedSaveFile(TaskThreadManager& thrdman);
    };
}
