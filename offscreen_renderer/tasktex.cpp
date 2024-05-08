#include "tasktex.hpp"
#include "unibmp.hpp"

using namespace UniformBitmap;

namespace RenderTaskSolver
{
    LoadTextureError::LoadTextureError(std::string what) noexcept:
        std::runtime_error(what)
    {
    }

    SaveTextureError::SaveTextureError(std::string what) noexcept :
        std::runtime_error(what)
    {
    }

    class BindTexture
    {
    protected:
        Context& gl;
        GLenum target;
    public:
        BindTexture() = delete;
        BindTexture(const BindTexture& c) = delete;
        inline BindTexture(Context& gl, GLenum target, GLuint texture) :
            gl(gl), target(target)
        {
            gl.BindTexture(target, texture);
        }
        inline ~BindTexture()
        {
            gl.BindTexture(target, 0);
        }
    };

    TaskTexture::TaskTexture(Context& gl, std::string Name, std::string LoadFrom, TexFileFormat Format) :
        gl(gl), Name(Name), TF(TextureFormat::Unknown), SaveFormat(Format),
        Width(0), Height(0), HasContent(false)
    {
        gl.GenTextures(1, &glTex);
        BindTexture Binding(gl, gl.TEXTURE_2D, glTex);
        try
        {
            switch (Format)
            {
            case TexFileFormat::BMP:
            case TexFileFormat::PNG:
            case TexFileFormat::JPG:
            case TexFileFormat::GIF:
            case TexFileFormat::TGA:
            case TexFileFormat::PPM:
            case TexFileFormat::PIC:
            case TexFileFormat::PNM:
            case TexFileFormat::PSD:
                if (IsImage16bpps(LoadFrom))
                {
                    TF = TextureFormat::RGBA16;
                    Image_RGBA16 bmp(LoadFrom);
                    Width = bmp.GetWidth();
                    Height = bmp.GetHeight();
                    gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA16, Width, Height, 0, gl.BGRA, gl.UNSIGNED_SHORT, bmp.GetBitmapDataPtr());
                    HasContent = true;
                }
                else
                {
                    TF = TextureFormat::RGBA8;
                    Image_RGBA8 bmp(LoadFrom);
                    Width = bmp.GetWidth();
                    Height = bmp.GetHeight();
                    gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA, Width, Height, 0, gl.BGRA, gl.UNSIGNED_BYTE, bmp.GetBitmapDataPtr());
                    HasContent = true;
                }
                break;
            case TexFileFormat::HDR:
                if (1)
                {
                    TF = TextureFormat::RGBA32F;
                    Image_RGBA32F bmp(LoadFrom);
                    Width = bmp.GetWidth();
                    Height = bmp.GetHeight();
                    gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA32F, Width, Height, 0, gl.BGRA, gl.FLOAT, bmp.GetBitmapDataPtr());
                    HasContent = true;
                }
                break;
            case TexFileFormat::Unknown:
            default:
                throw std::invalid_argument("Unknown format");
            }
        }
        catch (const LoadImageError& e)
        {
            throw LoadTextureError(e.what());
        }
        gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    }

    TaskTexture::TaskTexture(Context& gl, std::string Name, uint32_t Width, uint32_t Height, TextureFormat Format) :
        gl(gl), Name(Name), Width(Width), Height(Height), TF(Format), HasContent(false), SaveFormat(TexFileFormat::Unknown)
    {
        gl.GenTextures(1, &glTex);
        BindTexture Binding(gl, gl.TEXTURE_2D, glTex);
        switch (TF)
        {
        case TextureFormat::RGBA8:
            gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA, Width, Height, 0, gl.BGRA, gl.UNSIGNED_BYTE, nullptr);
            break;
        case TextureFormat::RGBA16:
            gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA16, Width, Height, 0, gl.BGRA, gl.UNSIGNED_SHORT, nullptr);
            break;
        case TextureFormat::RGBA32I:
            gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA32I, Width, Height, 0, gl.BGRA, gl.UNSIGNED_INT, nullptr);
            break;
        case TextureFormat::RGBA32F:
            gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA32F, Width, Height, 0, gl.BGRA, gl.FLOAT, nullptr);
            break;
        default:
            gl.BindTexture(gl.TEXTURE_2D, 0);
            throw std::invalid_argument("Unknown format");
        }
        gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    }

    TaskTexture::~TaskTexture()
    {
        gl.DeleteTextures(1, &glTex);
    }

    void TaskTexture::SaveFile()
    {
        BindTexture Binding(gl, gl.TEXTURE_2D, glTex);

        try
        {
            switch (SaveFormat)
            {
            case TexFileFormat::BMP:
            case TexFileFormat::PNG:
            case TexFileFormat::JPG:
            case TexFileFormat::TGA:
                if (1)
                {
                    Image_RGBA8 bmp(Width, Height);
                    gl.GetTexImage(gl.TEXTURE_2D, 0, gl.BGRA, gl.UNSIGNED_BYTE, reinterpret_cast<void*>(bmp.GetBitmapDataPtr()));
                    switch (SaveFormat)
                    {
                    case TexFileFormat::BMP: bmp.SaveToBmp32(SavePath); break;
                    case TexFileFormat::PNG: bmp.SaveToPNG(SavePath); break;
                    case TexFileFormat::JPG: bmp.SaveToJPG(SavePath, 100); break;
                    case TexFileFormat::TGA: bmp.SaveToTGA(SavePath); break;
                    }
                }
                break;
            case TexFileFormat::HDR:
                if (1)
                {
                    Image_RGBA32F bmp(Width, Height);
                    gl.GetTexImage(gl.TEXTURE_2D, 0, gl.BGRA, gl.FLOAT, reinterpret_cast<void*>(bmp.GetBitmapDataPtr()));
                    bmp.SaveToHDR(SavePath);
                }
                break;
            case TexFileFormat::Unknown:
                throw std::invalid_argument("Unknown format");
            default:
                throw std::invalid_argument("Unsupported format");
            }
        }
        catch (const SaveImageError& e)
        {
            throw SaveTextureError(e.what());
        }
    }
}
