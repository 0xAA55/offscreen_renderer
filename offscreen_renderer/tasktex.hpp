#pragma once
#include <string>
#include <stdexcept>

#include "glcontext.hpp"

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
        LoadTextureError(std::string what) noexcept;
    };

    class SaveTextureError : public std::runtime_error
    {
    public:
        SaveTextureError(std::string what) noexcept;
    };

    class TaskTexture
    {
    protected:
        Context& gl;
        std::string Name;
        GLuint glTex;
        TextureFormat TF;
        uint32_t Width;
        uint32_t Height;

    public:
        inline const std::string& GetName() const { return Name; }
        inline uint32_t GetWidth() const { return Width; }
        inline uint32_t GetHeight() const { return Height; }
        inline TextureFormat GetFormat() const { return TF; }
        inline operator GLuint() const { return glTex; }

        std::string SavePath;
        TexFileFormat SaveFormat;
        bool HasContent;

        TaskTexture(Context& gl, std::string Name, std::string LoadFrom, TexFileFormat Format);
        TaskTexture(Context& gl, std::string Name, uint32_t Width, uint32_t Height, TextureFormat Format);
        ~TaskTexture();

        void SaveFile();
    };
}
