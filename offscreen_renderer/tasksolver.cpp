#include "tasksolver.hpp"
#include "rtask.hpp"
#include "strutil.hpp"
#include <sstream>
#include <filesystem>

namespace RenderTaskSolver
{
    InvalidTaskConfig::InvalidTaskConfig(std::string what) noexcept :
        std::runtime_error(what)
    {
    }

    static TexFileFormat GetFormatByExt(std::string path)
    {
        if (ic_equal(path.substr(path.find_last_of(".") + 1), "bmp")) return TexFileFormat::BMP;
        else if (ic_equal(path.substr(path.find_last_of(".") + 1), "png")) return TexFileFormat::PNG;
        else if (ic_equal(path.substr(path.find_last_of(".") + 1), "jpg")) return TexFileFormat::JPG;
        else if (ic_equal(path.substr(path.find_last_of(".") + 1), "gif")) return TexFileFormat::GIF;
        else if (ic_equal(path.substr(path.find_last_of(".") + 1), "tga")) return TexFileFormat::TGA;
        else if (ic_equal(path.substr(path.find_last_of(".") + 1), "ppm")) return TexFileFormat::PPM;
        else if (ic_equal(path.substr(path.find_last_of(".") + 1), "pic")) return TexFileFormat::PIC;
        else if (ic_equal(path.substr(path.find_last_of(".") + 1), "pnm")) return TexFileFormat::PNM;
        else if (ic_equal(path.substr(path.find_last_of(".") + 1), "psd")) return TexFileFormat::PSD;
        else if (ic_equal(path.substr(path.find_last_of(".") + 1), "hdr")) return TexFileFormat::HDR;
        else
        {
            return TexFileFormat::Unknown;
        }
    }

    static TextureFormat GetTexFormatByStr(std::string fmt)
    {
        trim(fmt);
        if (fmt.size() == 0 || ic_equal(fmt, "RGBA8"))
        {
            return TextureFormat::RGBA8;
        }
        else if (ic_equal(fmt, "RGBA16"))
        {
            return TextureFormat::RGBA16;
        }
        else if (ic_equal(fmt, "RGBA32") || ic_equal(fmt, "RGBA32I"))
        {
            return TextureFormat::RGBA32I;
        }
        else if (ic_equal(fmt, "RGBA32F"))
        {
            return TextureFormat::RGBA32F;
        }
        else
        {
            return TextureFormat::Unknown;
        }
    }

    static TaskType GetTaskTypeByStr(std::string TypeStr)
    {
        trim(TypeStr);
        if (TypeStr.size() == 0 || ic_equal(TypeStr, "Draw"))
        {
            return TaskType::Task_Draw;
        }
        else if (ic_equal(TypeStr, "Compute"))
        {
            return TaskType::Task_Compute;
        }
        else
        {
            std::stringstream ss;
            ss << "Unknown task type `" << TypeStr << "`. Task type should be `Draw` or `Compute`, default is `Draw`.";
            throw InvalidTaskConfig(ss.str());
        }
    }

    TaskSolver::TaskSolver(Context& gl, const IniFile& TaskConf) :
        gl(gl),
        Config(TaskConf),
        BBVB(gl, std::vector<float>
            {
                -1, -1,
                1, -1,
                -1, 1,
                1, 1
            }, gl.STATIC_DRAW),
        BBEB(gl, std::vector<uint8_t>{ 0, 1, 2, 1, 3, 2 }, gl.STATIC_DRAW)
    {

        auto TaskList = split(Config.sections["common"]["tasks"], ",");
        for (auto& tn : TaskList)
        {
            trim(tn);
            auto& td = Config.sections[tn];
            auto type = GetTaskTypeByStr(td["type"]);
            auto inputs = split(td["input"], ",");
            auto outputs = split(td["output"], ",");
            auto shader_storages = split(td["storage"], ",");

            std::shared_ptr<TaskShader> shader;
            std::shared_ptr<RenderTask> task;
            switch (type)
            {
            default:
            case TaskType::Task_Draw:
                shader = std::make_shared<TaskShaderDraw>(gl, td["shader"], true);
                task = std::make_shared<RenderTaskDraw>(*this, shader, inputs, outputs, shader_storages);
                break;
            case TaskType::Task_Compute:
                if (1)
                {
                    uint32_t gx, gy, gz;
                    if (sscanf(td["groups"].c_str(), "%u,%u,%u", &gx, &gy, &gz) != 3)
                    {
                        std::stringstream ss;
                        ss << "Invalid config `groups` for compute task `" << tn << "`: " << td["groups"] << std::endl << "`groups` should be 3 numbers separated by comma.";
                        throw InvalidTaskConfig(ss.str());
                    }
                    shader = std::make_shared<TaskShaderCompute>(gl, td["shader"], true);
                    task = std::make_shared<RenderTaskCompute>(*this, shader, inputs, outputs, shader_storages, gx, gy, gz);
                }
                break;
            }

            Tasks.push_back(task);

            for (auto& i : inputs)
            {
                trim(i);
                if (i.size()) TextureSet.insert(i);
            }
            for (auto& i : outputs)
            {
                trim(i);
                if (i.size()) TextureSet.insert(i);
            }
            for (auto& i : shader_storages)
            {
                trim(i);
                if (i.size()) ShaderStorageSet.insert(i);
            }
        }

        for (auto& tn : TextureSet)
        {
            auto& td = Config.sections[tn];
            auto& spath = td["save"];
            auto sfmt = GetFormatByExt(spath);
            std::shared_ptr<TaskTexture> t;
            if (td.contains("load"))
            {
                auto& lpath = td["load"];
                t = std::make_shared<TaskTexture>(gl, tn, lpath, GetFormatByExt(lpath));
            }
            if (td.contains("size"))
            {
                int w, h;
                auto size_str = td["size"];
                auto sizesplit = split(size_str, ',');
                auto badsize_error = InvalidTaskConfig((std::stringstream() << "Invalid attrib `size=" << size_str << "` of texture `" << tn << "`").str());
                if (sizesplit.size() != 2)
                    throw badsize_error;
                else
                {
                    trim(sizesplit[0]);
                    trim(sizesplit[1]);
                    w = std::stoi(sizesplit[0]);
                    h = std::stoi(sizesplit[0]);
                    if (w <= 0 || h <= 0) throw badsize_error;
                }
                if (t)
                {
                    if (static_cast<int>(t->GetWidth()) != w ||
                        static_cast<int>(t->GetHeight()) != h)
                    {
                        throw InvalidTaskConfig((std::stringstream() << "Size of the texture `" << tn << "` is " << t->GetWidth() << "x" << t->GetHeight() << ", it doesn't match the attrib `size=" << size_str << "`").str());
                    }
                }
                else
                {
                    t = std::make_shared<TaskTexture>(gl, tn, w, h, GetTexFormatByStr(td["format"]));
                }
            }
            if (!t)
            {
                throw InvalidTaskConfig((std::stringstream() << "For texture `" << tn << "` must have a `load` attrib or a `size` attrib.").str());
            }
            t->SavePath = spath;
            t->SaveFormat = sfmt;
            TextureMap.insert({ tn, t });
        }

        for (auto& sn : ShaderStorageSet)
        {
            auto& sd = Config.sections[sn];
            std::shared_ptr<TaskShaderStorage> s;
            if (sd.contains("load"))
            {
                s = std::make_shared<TaskShaderStorage>(gl, sd["load"]);
            }
            if (sd.contains("size"))
            {
                int64_t size;
                auto& size_str = sd["size"];
                auto badsize_error = InvalidTaskConfig((std::stringstream() << "Invalid attrib `size=" << size_str << "` of shader storage `" << sn << "`").str());
                trim(size_str);
                if (size_str.size() == 0) throw badsize_error;
                if (size_str.starts_with("0x"))
                {
                    size = std::stoll(&size_str[2], nullptr, 16);
                }
                else if (size_str.starts_with("0"))
                {
                    size = std::stoll(&size_str[1], nullptr, 8);
                }
                else
                {
                    size = std::stoll(size_str);
                }
                if (size <= 0) throw badsize_error;
                if (s)
                {
                    if (s->GetSize() != size)
                    {
                        throw InvalidTaskConfig((std::stringstream() << "Size of the shader storage `" << sn << "` is " << s->GetSize() << " bytes, it doesn't match the attrib `size=" << size_str << "`").str());
                    }
                }
                else
                {
                    s = std::make_shared<TaskShaderStorage>(gl, nullptr, size);
                }
            }
            if (!s)
            {
                throw InvalidTaskConfig((std::stringstream() << "For shader storage `" << sn << "` must have a `load` attrib or a `size` attrib.").str());
            }
            s->SavePath = sd["save"];
            ShaderStorageMap.insert({ sn, s });
        }

        for (auto& t : Tasks)
        {
            t->OnLoadingFinished();
        }
    }

    TaskSolver::TaskSolver(Context& gl, std::string ConfFile) :
        TaskSolver(gl, IniFile(ConfFile))
    {
    }

    void TaskSolver::SolveTasks()
    {
        for (auto& t : Tasks)
        {
            t->Process();
        }
        for (auto& tkv : TextureMap)
        {
            auto& tex = tkv.second;
            if (tex->SavePath.size())
            {
                tex->SaveFile();
            }
        }
        for (auto& skv : ShaderStorageMap)
        {
            auto& ss = skv.second;
            if (ss->SavePath.size())
            {
                ss->Save(ss->SavePath);
            }
        }
    }
}
