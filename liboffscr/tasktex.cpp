#include "tasktex.hpp"
#include "unibmp.hpp"
#include "tasksolver.hpp"
#include <sstream>
#include <iostream>

namespace RenderTaskSolver
{
    using namespace UniformBitmap;

    LoadTextureError::LoadTextureError(const std::string& what) noexcept:
        std::runtime_error(what)
    {
    }

    SaveTextureError::SaveTextureError(const std::string& what) noexcept :
        std::runtime_error(what)
    {
    }

    class BindTexture
    {
    protected:
        const Context& gl;
        GLenum target;
    public:
        BindTexture() = delete;
        BindTexture(const BindTexture& c) = delete;
        inline BindTexture(const Context& gl, GLenum target, GLuint texture) :
            gl(gl), target(target)
        {
            gl.BindTexture(target, texture);
        }
        inline ~BindTexture()
        {
            gl.BindTexture(target, 0);
        }
    };

    TaskTexture::TaskTexture(TaskSolver& Solver, const std::string& Name, const std::string& LoadFrom, TexFileFormat Format) :
        Solver(Solver), gl(Solver.gl),
        Name(Name), TF(TextureFormat::Unknown), SaveFormat(Format),
        Width(0), Height(0), HasContent(false),
        DontKeep(false)
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

    TaskTexture::TaskTexture(TaskSolver& Solver, const std::string& Name, uint32_t Width, uint32_t Height, TextureFormat Format) :
        Solver(Solver), gl(Solver.gl),
        Name(Name), Width(Width), Height(Height), TF(Format), HasContent(false), SaveFormat(TexFileFormat::Unknown),
        DontKeep(false)
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

    static void ReportThreadFuncFinished(std::string SavePath)
    {
        std::stringstream ss;
        ss << "Threaded saved file `" << SavePath << "`." << std::endl;
        std::cout << (ss).str();
    }

    static void SaveBmp32Func(std::unique_ptr<Image_RGBA8> bmp, std::string SavePath, bool ReportThreadedFinished)
    {
        bmp->SaveToBmp32(SavePath);
        if (ReportThreadedFinished) ReportThreadFuncFinished(SavePath);
    }

    static void SavePNGFunc(std::unique_ptr<Image_RGBA8> bmp, std::string SavePath, bool ReportThreadedFinished)
    {
        bmp->SaveToPNG(SavePath);
        if (ReportThreadedFinished) ReportThreadFuncFinished(SavePath);
    }

    static void SaveJPGFunc(std::unique_ptr<Image_RGBA8> bmp, std::string SavePath, bool ReportThreadedFinished)
    {
        bmp->SaveToJPG(SavePath, 100);
        if (ReportThreadedFinished) ReportThreadFuncFinished(SavePath);
    }

    static void SaveTGAFunc(std::unique_ptr<Image_RGBA8> bmp, std::string SavePath, bool ReportThreadedFinished)
    {
        bmp->SaveToTGA(SavePath);
        if (ReportThreadedFinished) ReportThreadFuncFinished(SavePath);
    }

    static void SaveHDRFunc(std::unique_ptr<Image_RGBA32F> bmp, std::string SavePath, bool ReportThreadedFinished)
    {
        bmp->SaveToHDR(SavePath);
        if (ReportThreadedFinished) ReportThreadFuncFinished(SavePath);
    }

    static const std::unordered_map<TexFileFormat, void(*)(std::unique_ptr<Image_RGBA8>, std::string SavePath, bool Threaded)> SaveRGBA8Funcs = {
        {TexFileFormat::BMP, SaveBmp32Func},
        {TexFileFormat::PNG, SavePNGFunc},
        {TexFileFormat::JPG, SaveJPGFunc},
        {TexFileFormat::TGA, SaveTGAFunc}
    };

    static const std::unordered_map<TexFileFormat, void(*)(std::unique_ptr<Image_RGBA32F>, std::string SavePath, bool Threaded)> SaveRGBA32FFuncs = {
        {TexFileFormat::HDR, SaveHDRFunc}
    };

    ThreadIdType TaskTexture::SaveFileInternal(TaskThreadManager* thrdman)
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
                    auto bmp = std::make_unique<Image_RGBA8>(Width, Height);
                    gl.GetTexImage(gl.TEXTURE_2D, 0, gl.BGRA, gl.UNSIGNED_BYTE, reinterpret_cast<void*>(bmp->GetBitmapDataPtr()));
                    if (!thrdman)
                    {
                        SaveRGBA8Funcs.at(SaveFormat)(std::move(bmp), SavePath, false);
                        return ThreadIdType();
                    }
                    else
                    {
                        return thrdman->AddThread(std::make_unique<std::jthread>(SaveRGBA8Funcs.at(SaveFormat), std::move(bmp), SavePath, true && Solver.Verbose));
                    }
                }
                break;
            case TexFileFormat::HDR:
                if (1)
                {
                    auto bmp = std::make_unique<Image_RGBA32F>(Width, Height);
                    gl.GetTexImage(gl.TEXTURE_2D, 0, gl.BGRA, gl.FLOAT, reinterpret_cast<void*>(bmp->GetBitmapDataPtr()));
                    if (!thrdman)
                    {
                        SaveRGBA32FFuncs.at(SaveFormat)(std::move(bmp), SavePath, false);
                        return ThreadIdType();
                    }
                    else
                    {
                        return thrdman->AddThread(std::make_unique<std::jthread>(SaveRGBA32FFuncs.at(SaveFormat), std::move(bmp), SavePath, true && Solver.Verbose));
                    }
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

    void TaskTexture::SaveFile()
    {
        SaveFileInternal(nullptr);
    }

    ThreadIdType TaskTexture::ThreadedSaveFile(TaskThreadManager& thrdman)
    {
        return SaveFileInternal(&thrdman);
    }
}
