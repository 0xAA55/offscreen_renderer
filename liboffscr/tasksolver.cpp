#include "tasksolver.hpp"
#include "rtask.hpp"
#include "strutil.hpp"
#include <set>
#include <string>
#include <cctype>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <filesystem>
#include <unordered_map>

namespace RenderTaskSolver
{
    using namespace StringUtil;

    InvalidTaskConfig::InvalidTaskConfig(const std::string& what) noexcept :
        std::runtime_error(what)
    {
    }

    ReferencedConfigNotFound::ReferencedConfigNotFound(const std::string& what, const std::string& ReferenceSection, const std::string& ReferenceKey) noexcept :
        ReferencedConfigNotFound(std::out_of_range(what), ReferenceSection, ReferenceKey)
    {
    }

    ReferencedConfigNotFound::ReferencedConfigNotFound(const std::out_of_range& oor, const std::string& ReferenceSection, const std::string& ReferenceKey) noexcept :
        std::out_of_range(oor), ReferenceSection(ReferenceSection), ReferenceKey(ReferenceKey)
    {
    }

    static const std::unordered_map<std::string, TexFileFormat> ExtToTexFileFormatEnum =
    {
        {"bmp", TexFileFormat::BMP},
        {"png", TexFileFormat::PNG},
        {"jpg", TexFileFormat::JPG},
        {"gif", TexFileFormat::GIF},
        {"tga", TexFileFormat::TGA},
        {"ppm", TexFileFormat::PPM},
        {"pic", TexFileFormat::PIC},
        {"pnm", TexFileFormat::PNM},
        {"psd", TexFileFormat::PSD},
        {"hdr", TexFileFormat::HDR},
    };

    static const std::unordered_map<std::string, TextureFormat> StrToTextureFormatEnum =
    {
        {"", TextureFormat::RGBA8},
        {"RGBA8", TextureFormat::RGBA8},
        {"RGBA16", TextureFormat::RGBA16},
        {"RGBA32", TextureFormat::RGBA32I},
        {"RGBA32I", TextureFormat::RGBA32I},
        {"RGBA32F", TextureFormat::RGBA32F},
    };

    static const std::unordered_map<std::string, TaskType> StrToTaskTypeEnum =
    {
        {"", TaskType::Task_Draw},
        {"draw", TaskType::Task_Draw},
        {"Draw", TaskType::Task_Draw},
        {"compute", TaskType::Task_Compute},
        {"Compute", TaskType::Task_Compute},
    };

    static TexFileFormat GetFormatByExt(const std::string& path)
    {
        auto ExtPos = path.find_last_of(".") + 1;
        auto ext = path.substr(ExtPos);
        std::transform(ext.begin(), ext.end(), ext.begin(),
            [](unsigned char c) { return std::tolower(c); });
        try
        {
            return ExtToTexFileFormatEnum.at(ext);
        }
        catch (const std::out_of_range&)
        {
            return TexFileFormat::Unknown;
        }
    }

    static TextureFormat GetTexFormatByStr(std::string fmt)
    {
        TrimInPlace(fmt);
        try
        {
            return StrToTextureFormatEnum.at(fmt);
        }
        catch (const std::out_of_range&)
        {
            return TextureFormat::Unknown;
        }
    }

    static TaskType GetTaskTypeByStr(std::string TypeStr)
    {
        TrimInPlace(TypeStr);
        try
        {
            return StrToTaskTypeEnum.at(TypeStr);
        }
        catch (const std::out_of_range&)
        {
            std::stringstream ss;
            ss << "Unknown task type `" << TypeStr << "`. Task type should be `Draw` or `Compute`, default is `Draw`.";
            throw InvalidTaskConfig(ss.str());
        }
    }

    std::string TaskSolver::GetConfigValue(const std::string& SectionName, const std::string& Key, const bool GetReferencedValue)
    {
        std::string s = Trim(SectionName);
        std::string k = Trim(Key);
        std::string Value = Trim(Config.sections[s][k]);
        if (!GetReferencedValue) return Value;
        std::stringstream ss;
        ss << s << "." << k;
        std::set<std::string> RecursiveReferenceCheck = { ss.str()};
        ss = std::stringstream();
        while (Value[0] == '*')
        {
            auto Fwd = Split(Value.substr(1), '.');
            if (Fwd.size() != 2)
            {
                ss << "Invalid config reference `" << Value << "`";
                throw InvalidTaskConfig(ss.str());
            }
            s = Trim(Fwd[0]);
            k = Trim(Fwd[1]);
            ss << s << "." << k;
            auto s_k = ss.str();
            ss = std::stringstream();
            if (RecursiveReferenceCheck.contains(s_k))
            {
                ss << "Recursive reference detected:" << std::endl;
                for (auto& i : RecursiveReferenceCheck) ss << " *" << i << std::endl;
                throw InvalidTaskConfig(ss.str());
            }
            RecursiveReferenceCheck.insert(s_k);
            try
            {
                Value = Trim(Config.sections.at(s).at(k));
            }
            catch (const std::out_of_range& e)
            {
                throw ReferencedConfigNotFound(e, s, k);
            }
        }
        return Value;
    }

    bool TaskSolver::GetConfigValueBoolean(const std::string& SectionName, const std::string& Key, bool Default, const bool GetReferencedValue)
    {
        auto StringValue = GetConfigValue(SectionName, Key, GetReferencedValue);
        if (StringValue.size() == 0) return Default;
        else return IsTrueWord(StringValue);
    }

    void TaskSolver::CreateAppliedBatchSection(const std::string& SectionName, const std::string& BatchId)
    {
        auto& template_data = Config.sections[SectionName];
        IniFile::SectionType new_section;
        for (auto& kv : template_data)
        {
            auto &k = kv.first;
            auto v = GetConfigValue(SectionName, k, false);
            new_section[k] = Replace(v, "<batch>", BatchId);
        }
        auto new_section_name = Replace(SectionName, "<batch>", BatchId);
        if (Config.sections.contains(new_section_name))
        {
            std::stringstream ss;
            ss << "Batch process error: conflict section name `" << new_section_name << "`";;
            throw InvalidTaskConfig(ss.str());
        }
        Config.sections[new_section_name] = new_section;
    }

    std::shared_ptr<TaskShader> TaskSolver::LoadDrawCommandShader(const std::string& LoadPath)
    {
        auto lpath = std::filesystem::path(LoadPath);
        if (lpath.is_relative()) lpath = WorkDir / lpath;
        
        if (Verbose)
        {
            std::cout << "Creating draw command shader from `" << std::filesystem::absolute(lpath).string() << "`." << std::endl;
        }

        return static_cast<std::shared_ptr<TaskShader>> (std::make_shared<TaskShaderDraw>(gl, LoadPath, lpath.string(), true));
    }

    std::shared_ptr<TaskShader> TaskSolver::LoadComputeShader(const std::string& LoadPath)
    {
        auto lpath = std::filesystem::path(LoadPath);
        if (lpath.is_relative()) lpath = WorkDir / lpath;

        if (Verbose)
        {
            std::cout << "Creating compute shader from `" << std::filesystem::absolute(lpath).string() << "`." << std::endl;
        }

        return static_cast<std::shared_ptr<TaskShader>> (std::make_shared<TaskShaderCompute>(gl, LoadPath, lpath.string(), true));
    }

    void TaskSolver::CreateTexture(const std::string& tn)
    {
        auto spath = GetConfigValue(tn, "save");
        auto lpath = GetConfigValue(tn, "load");
        auto size_str = GetConfigValue(tn, "size");
        auto uniform = GetConfigValue(tn, "uniform");
        auto shader_output_name = GetConfigValue(tn, "shader_output_name");
        if (!uniform.size()) uniform = tn;
        std::shared_ptr<TaskTexture> t;
        std::stringstream ss;
        if (lpath.size())
        {
            auto lp = std::filesystem::path(lpath);
            if (lp.is_relative()) lp = WorkDir / lp;
            lpath = lp.string();

            if (Verbose)
            {
                std::cout << "Loading texture for `" << tn << "` from `" << std::filesystem::absolute(lpath).string() << "`" << std::endl;
            }

            t = std::make_shared<TaskTexture>(*this, tn, shader_output_name, lpath, GetFormatByExt(lpath));

            ss << t->GetWidth() << "," << t->GetHeight();
            Config.sections[tn]["size"] = ss.str();
            ss = std::stringstream();
        }
        if (spath.size())
        {
            auto sp = std::filesystem::path(spath);
            if (sp.is_relative()) sp = WorkDir / sp;
            spath = sp.string();

            if (Verbose)
            {
                std::cout << "Texture `" << tn << "` will be saved to `" << std::filesystem::absolute(spath).string() << "`" << std::endl;
            }
        }
        if (size_str.size())
        {
            int w, h;
            auto size_str = GetConfigValue(tn, "size");
            ss << "Invalid attrib `size=" << size_str << "` of texture `" << tn << "`";
            auto badsize_string = ss.str();
            try
            {
                auto wh = ParseIntFields(size_str, ",", 2);
                w = wh[0];
                h = wh[1];
                if (w <= 0 || h <= 0) throw InvalidTaskConfig(badsize_string);
            }
            catch (const ParseError& e)
            {
                ss << badsize_string << ": " << e.what();
                throw InvalidTaskConfig(ss.str());
            }
            if (t)
            {
                if (static_cast<int>(t->GetWidth()) != w ||
                    static_cast<int>(t->GetHeight()) != h)
                {
                    ss << "Size of the texture `" << tn << "` is " << t->GetWidth() << "x" << t->GetHeight() << ", it doesn't match the attrib `size=" << size_str << "`";
                    throw InvalidTaskConfig(ss.str());
                }
                if (Verbose)
                {
                    std::cout << "Created texture `" << tn << "` by size " << w << "x" << h << " will not be changed." << std::endl;
                }
            }
            else
            {
                if (Verbose)
                {
                    std::cout << "Creating texture for `" << tn << "` by size " << w << "x" << h << "." << std::endl;
                }
                t = std::make_shared<TaskTexture>(*this, tn, shader_output_name, w, h, GetTexFormatByStr(GetConfigValue(tn, "format")));
            }
        }
        if (!t)
        {
            ss << "For texture `" << tn << "` must have a `load` attrib or a `size` attrib.";
            throw InvalidTaskConfig(ss.str());
        }
        t->UniformName = uniform;
        t->DontKeep = GetConfigValueBoolean(tn, "dontkeep", false);
        t->SavePath = spath;
        t->SaveFormat = GetFormatByExt(t->SavePath);
        TextureMap[tn] = t;
    }

    void TaskSolver::CreateTextureRecursive(const std::string& tn)
    {
        while (!TextureMap[tn])
        {
            try
            {
                CreateTexture(tn);
            }
            catch (const ReferencedConfigNotFound& rcnf)
            {
                CreateTextureRecursive(rcnf.GetReferenceSection());
            }
        }
    }

    void TaskSolver::CreateShaderStorageRecursive(const std::string& ssn)
    {
        while (!ShaderStorageMap[ssn])
        {
            try
            {
                CreateShaderStorage(ssn);
            }
            catch (const ReferencedConfigNotFound& rcnf)
            {
                CreateShaderStorageRecursive(rcnf.GetReferenceSection());
            }
        }
    }

    void TaskSolver::CreateShaderStorage(const std::string& ssn)
    {
        auto lpath = GetConfigValue(ssn, "load");
        auto size_str = GetConfigValue(ssn, "size");
        std::shared_ptr<TaskShaderStorage> s;
        std::stringstream ss;
        if (lpath.size())
        {
            auto lp = std::filesystem::path(lpath);
            if (lp.is_relative()) lp = WorkDir / lp;
            lpath = lp.string();

            if (Verbose)
            {
                std::cout << "Loading shader storage for `" << ssn << "` from `" << std::filesystem::absolute(lpath).string() << "`" << std::endl;
            }

            s = std::make_shared<TaskShaderStorage>(gl, ssn, lpath);
        }
        if (size_str.size())
        {
            int64_t size = ParseInt64Field(size_str);
            ss << "Invalid attrib `size=" << size_str << "` of shader storage `" << ssn << "`";
            auto badsize_error = InvalidTaskConfig(ss.str());
            ss = std::stringstream();
            if (size <= 0) throw badsize_error;
            if (s)
            {
                if (s->GetSize() != size)
                {
                    ss << "Size of the shader storage `" << ssn << "` is " << s->GetSize() << " bytes, it doesn't match the attrib `size=" << size_str << "`";
                    throw InvalidTaskConfig(ss.str());
                }
                if (Verbose)
                {
                    std::cout << "Created shader storage `" << ssn << "` by size " << size << " bytes will not be changed." << std::endl;
                }
            }
            else
            {
                if (Verbose)
                {
                    std::cout << "Creating shader storage `" << ssn << "` by size " << size << " bytes." << std::endl;
                }

                s = std::make_shared<TaskShaderStorage>(gl, ssn, nullptr, size);
            }
        }
        if (!s)
        {
            ss << "For shader storage `" << ssn << "` must have a `load` attrib or a `size` attrib.";
            throw InvalidTaskConfig(ss.str());
        }
        s->DontKeep = GetConfigValueBoolean(ssn, "dontkeep", false);
        s->SavePath = GetConfigValue(ssn, "save");
        ShaderStorageMap[ssn] = s;
    }

    void TaskSolver::CheckTextureDontKeep(const std::string& tn)
    {
        auto& tex = TextureMap[tn];
        if (tex && tex->DontKeep)
        {
            if (Verbose)
            {
                std::cout << "Texture `" << tn << "` is marked as `dontkeep`, unloading now." << std::endl;
            }
            CheckSaveTexture(tn);
            tex.reset();
        }
    }

    void TaskSolver::CheckShaderStorageDontKeep(const std::string& ssn)
    {
        auto& ss = ShaderStorageMap[ssn];
        if (ss && ss->DontKeep)
        {
            if (Verbose)
            {
                std::cout << "Shader storage `" << ssn << "` is marked as `dontkeep`, unloading now." << std::endl;
            }
            CheckSaveShaderStorage(ssn);
            ss.reset();
        }
    }

    void TaskSolver::CheckSaveTexture(const std::string& tn)
    {
        auto& tex = TextureMap[tn];

        if (tex && tex->SavePath.size())
        {
            if (Verbose)
            {
                std::cout << "Saving texture `" << tn << "` to `"<< std::filesystem::absolute(tex->SavePath).string() <<"`." << std::endl;
            }
            tex->ThreadedSaveFile(TaskThreads);
        }
    }

    void TaskSolver::CheckSaveShaderStorage(const std::string& ssn)
    {
        auto& ss = ShaderStorageMap[ssn];

        if (ss && ss->SavePath.size())
        {
            if (Verbose)
            {
                std::cout << "Saving shader storage `" << ssn << "` to `" << std::filesystem::absolute(ss->SavePath).string() << "`." << std::endl;
            }
            ss->Save();
        }
    }

    TaskSolver::TaskSolver(const Context& gl, const IniFile& TaskConf, const std::filesystem::path& ConfigDir, const std::set<std::string>& Options) :
        gl(gl),
        Options(Options),
        Config(TaskConf),
        WorkDir(ConfigDir),
        BBVB(gl, std::vector<float>
            {
                -1, -1,
                1, -1,
                -1, 1,
                1, 1
            }, gl.STATIC_DRAW),
        BBEB(gl, std::vector<uint8_t>{ 0, 1, 2, 1, 3, 2 }, gl.STATIC_DRAW)
    {
        Verbose = Options.contains("-v");

        auto BatchConfig = GetConfigValue("common", "batch");
        BatchBI = 0;
        BatchEI = 0;
        BatchStep = 0;
        if (BatchConfig.size())
        {
            Batch = true;
            auto batch_fields = ParseIntFields(BatchConfig, ",");
            switch (batch_fields.size())
            {
            case 1:
                BatchBI = 0;
                BatchEI = batch_fields[0];
                BatchStep = 1;
                break;
            case 2:
                BatchBI = batch_fields[0];
                BatchEI = batch_fields[1];
                BatchStep = 1;
                break;
            case 3:
                BatchBI = batch_fields[0];
                BatchEI = batch_fields[1];
                BatchStep = batch_fields[2];
                break;
            }
            if ((BatchEI > BatchBI && BatchStep < 0) ||
                (BatchBI > BatchEI && BatchStep > 0) ||
                (BatchBI != BatchEI && BatchStep == 0))
            {
                std::stringstream ss;
                ss << "Invalid attrib `batch=" << BatchConfig << "` of section `common`";
                throw InvalidTaskConfig(ss.str());
            }

            if (Verbose)
            {
                std::cout << "Batch process from " << BatchBI << " to " << BatchEI << " step " << BatchStep << "." << std::endl;
            }
        }
        else
        {
            Batch = false;
        }

        std::unordered_set<std::string> TextureSet;
        std::unordered_set<std::string> ShaderStorageSet;

        auto TaskConfig = GetConfigValue("common", "tasks");
        auto TaskList = ParseUniqueFields(TaskConfig, ",");
        for (auto& tn : TaskList)
        {
            auto shadername = GetConfigValue(tn, "shader");
            auto type = GetTaskTypeByStr(GetConfigValue(tn, "type"));
            auto inputs = ParseUniqueFields(GetConfigValue(tn, "input"), ",");
            auto outputs = ParseUniqueFields(GetConfigValue(tn, "output"), ",");
            auto shader_storages = ParseUniqueFields(GetConfigValue(tn, "storage"), ",");

            std::shared_ptr<TaskShader>& shader = ShaderMap[shadername];
            std::unique_ptr<RenderTask> task;
            switch (type)
            {
            default:
            case TaskType::Task_Draw:
                if (!shader) shader = LoadDrawCommandShader(shadername);
                task = std::make_unique<RenderTaskDraw>(*this, tn, shader, inputs, outputs, shader_storages);
                break;
            case TaskType::Task_Compute:
                do
                {
                    uint32_t gx, gy, gz;
                    auto Groups = GetConfigValue(tn, "groups");
                    try
                    {
                        auto groups = ParseUIntFields(Groups, ",", 3);
                        gx = groups[0];
                        gy = groups[1];
                        gz = groups[2];
                    }
                    catch (const ParseError& e)
                    {
                        std::stringstream ss;
                        ss << "Invalid config `groups` for compute task `" << tn << "`:" << std::endl
                            << "`" << Groups << "`" << e.what() << std::endl
                            << "`groups` should be 3 numbers separated by comma.";
                        throw InvalidTaskConfig(ss.str());
                    }
                    if (!shader) shader = LoadComputeShader(shadername);
                    task = std::make_unique<RenderTaskCompute>(*this, tn, shader, inputs, outputs, shader_storages, gx, gy, gz);
                } while (false);
                break;
            }

            Tasks.push_back(std::move(task));

            for (auto& i : inputs) TextureSet.insert(i);
            for (auto& i : outputs) TextureSet.insert(i);
            for (auto& i : shader_storages) ShaderStorageSet.insert(i);
        }

        for (auto& tn : TextureSet)
        {
            if (HasSubString(tn, "batch")) continue;
            if (!GetConfigValueBoolean(tn, "lazyload", false))
            {
                try
                {
                    CreateTexture(tn);
                }
                catch (const ReferencedConfigNotFound& rcnf)
                {
                    if (Verbose)
                    {
                        std::cout << "Skipping creating texture `" << tn << "` because the referenced configuration `" << rcnf.GetReferenceSection() << "." << rcnf.GetReferenceKey() << "` is not initialized right now." << std::endl;
                    }
                }
            }
            else
            {
                if (Verbose)
                {
                    std::cout << "Texture `" << tn << "` is marked as lazyload and will not be load now." << std::endl;
                }
            }
        }

        for (auto& ssn : ShaderStorageSet)
        {
            if (HasSubString(ssn, "batch")) continue;
            if (!GetConfigValueBoolean(ssn, "lazyload", false))
            {
                CreateShaderStorage(ssn);
            }
            else
            {
                if (Verbose)
                {
                    std::cout << "Shader storage `" << ssn << "` is marked as lazyload and will not be load now." << std::endl;
                }
            }
        }
    }

    TaskSolver::TaskSolver(const Context& gl, const std::string& ConfFile, const std::set<std::string>& Options) :
        TaskSolver(gl, IniFile(ConfFile), std::filesystem::path(ConfFile).remove_filename(), Options)
    {
    }

    void TaskSolver::LoadLazyLoadDataForTask(RenderTask& task)
    {
        for (auto& tn : task.GetInputs()) CreateTextureRecursive(tn);
        for (auto& tn : task.GetOutputs()) CreateTextureRecursive(tn);
        for (auto& ss : task.GetShaderStorages()) CreateShaderStorageRecursive(ss);
    }

    void TaskSolver::FreeDontKeepDataForTask(RenderTask& task)
    {
        for (auto& tn : task.GetInputsAndOutputs())
        {
            CheckTextureDontKeep(tn);
        }
        for (auto& ssn : task.GetShaderStorages())
        {
            CheckShaderStorageDontKeep(ssn);
        }
    }

    void TaskSolver::SolveTasks()
    {
        for (auto& t : Tasks)
        {
            auto& tn = t->GetName();
            
            if (HasSubString(tn, "<batch>", false))
            {
                if (Verbose)
                {
                    std::cout << "Processing batch task `" << tn << "`." << std::endl;
                }

                auto shadername = GetConfigValue(tn, "shader");
                auto type = GetTaskTypeByStr(GetConfigValue(tn, "type"));

                for (int32_t i = BatchBI;
                    BatchStep > 0 ? i <= BatchEI :
                    BatchStep < 0 ? i >= BatchEI :
                    true; i += BatchStep)
                {
                    std::stringstream ss;
                    ss << i;
                    std::string batch_id = ss.str();
                    std::string batch_tn = Replace(tn, "<batch>", batch_id);
                    std::string batch_shadername = Replace(shadername, "<batch>", batch_id);
                    std::shared_ptr<TaskShader>& shader = ShaderMap[shadername];
                    std::unique_ptr<RenderTask> task;

                    std::set<std::string> batch_inputs;
                    std::set<std::string> batch_outputs;
                    std::set<std::string> batch_shaderstorages;

                    for (auto& i : t->GetInputs())
                    {
                        auto input_name = Replace(i, "<batch>", batch_id);
                        batch_inputs.insert(input_name);

                        if (!IsSameWord(i, input_name)) CreateAppliedBatchSection(i, batch_id);
                    }
                    for (auto& i : t->GetOutputs())
                    {
                        auto output_name = Replace(i, "<batch>", batch_id);
                        batch_outputs.insert(output_name);

                        if (!IsSameWord(i, output_name)) CreateAppliedBatchSection(i, batch_id);
                    }
                    for (auto& i : t->GetShaderStorages())
                    {
                        auto shaderstorage_name = Replace(i, "<batch>", batch_id);
                        batch_shaderstorages.insert(shaderstorage_name);

                        if (!IsSameWord(i, shaderstorage_name)) CreateAppliedBatchSection(i, batch_id);
                    }

                    if (Verbose)
                    {
                        std::cout << "Processing batch `" << batch_id << "`." << std::endl;
                    }

                    switch (type)
                    {
                    default:
                    case TaskType::Task_Draw:
                        if (!shader) shader = LoadDrawCommandShader(shadername);
                        task = std::make_unique<RenderTaskDraw>(*this, batch_tn, shader, batch_inputs, batch_outputs, batch_shaderstorages);
                        break;
                    case TaskType::Task_Compute:
                        if (1)
                        {
                            RenderTaskCompute& c = *dynamic_cast<RenderTaskCompute*>(t.get());
                            if (!shader) shader = LoadComputeShader(shadername);
                            task = std::make_unique<RenderTaskCompute>(*this, tn, shader, batch_inputs, batch_outputs, batch_shaderstorages, c.numGroupX, c.numGroupY, c.numGroupZ);
                        }
                        break;
                    }

                    LoadLazyLoadDataForTask(*task);
                    task->Process();
                    FreeDontKeepDataForTask(*task);

                    if (!BatchStep) break;
                }
            }
            else
            {
                LoadLazyLoadDataForTask(*t);
                t->Process();
                FreeDontKeepDataForTask(*t);
            }
        }
        for (auto& tn : TextureMap)
        {
            auto& tex = tn.second;
            if (!tex) continue;
            if (tex->SavePath.size())
            {
                tex->ThreadedSaveFile(TaskThreads);
                if (tex->DontKeep) tex.reset();
            }
        }
        for (auto& ssn : ShaderStorageMap)
        {
            auto& ss = ssn.second;
            if (!ss) continue;
            if (ss->SavePath.size())
            {
                ss->Save();
                if (ss->DontKeep) ss.reset();
            }
        }
        TaskThreads.Join();
    }
}
