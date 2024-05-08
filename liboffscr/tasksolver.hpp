#pragma once
#include <memory>
#include <stdexcept>
#include <filesystem>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "taskthread.hpp"
#include "inifile.hpp"
#include "tasktex.hpp"
#include "taskshader.hpp"
#include "tasksstorage.hpp"
#include "rtask.hpp"

namespace RenderTaskSolver
{
    using namespace GLObjects;

    class InvalidTaskConfig : public std::runtime_error
    {
    public:
        InvalidTaskConfig(const std::string& what) noexcept;
    };

    class ReferencedConfigNotFound : public std::out_of_range
    {
    protected:
        std::string ReferenceSection;
        std::string ReferenceKey;

    public:
        ReferencedConfigNotFound(const std::string& what, const std::string& ReferenceSection, const std::string& ReferenceKey) noexcept;
        ReferencedConfigNotFound(const std::out_of_range& oor, const std::string& ReferenceSection, const std::string& ReferenceKey) noexcept;

        inline const std::string& GetReferenceSection() const { return ReferenceSection ; }
        inline const std::string& GetReferenceKey() const { return ReferenceKey; }
    };

    class RenderTask;
    class RenderTaskDraw;
    class RenderTaskCompute;

    class TaskSolver
    {
    protected:
        friend class RenderTask;
        friend class RenderTaskDraw;
        friend class RenderTaskCompute;
        friend class TaskTexture;
        const Context& gl;
        IniFile Config;
        std::filesystem::path WorkDir;
        std::set<std::string> Options;
        std::vector<std::unique_ptr<RenderTask>> Tasks;
        std::unordered_map<std::string, std::shared_ptr<TaskShader>> ShaderMap;
        std::unordered_map<std::string, std::shared_ptr<TaskTexture>> TextureMap;
        std::unordered_map<std::string, std::shared_ptr<TaskShaderStorage>> ShaderStorageMap;

        ArrayBuffer BBVB;
        ElementArrayBuffer BBEB;

        std::string GetConfigValue(const std::string& SectionName, const std::string& Key, const bool GetReferencedValue = true);
        bool GetConfigValueBoolean(const std::string& SectionName, const std::string& Key, bool Default, const bool GetReferencedValue = true);

        std::shared_ptr<TaskShader> LoadDrawCommandShader(const std::string& LoadPath);
        std::shared_ptr<TaskShader> LoadComputeShader(const std::string& LoadPath);

        void CreateTexture(const std::string& tn);
        void CreateShaderStorage(const std::string& ssn);

        void CheckTextureDontKeep(const std::string& tn);
        void CheckShaderStorageDontKeep(const std::string& ssn);

        void CheckSaveTexture(const std::string& tn);
        void CheckSaveShaderStorage(const std::string& ssn);

        void CreateTextureRecursive(const std::string& tn);
        void CreateShaderStorageRecursive(const std::string& ssn);

        void LoadLazyLoadDataForTask(RenderTask& task);
        void FreeDontKeepDataForTask(RenderTask& task);

        void CreateAppliedBatchSection(const std::string& SectionName, const std::string& BatchId);

        bool Batch;
        int BatchBI;
        int BatchEI;
        int BatchStep;

        bool Verbose;

        TaskThreadManager TaskThreads;

    public:
        TaskSolver(const Context& gl, const IniFile& TaskConf, const std::filesystem::path& ConfigDir = "", const std::set<std::string>& Options = std::set<std::string>());
        TaskSolver(const Context& gl, const std::string& ConfFile, const std::set<std::string>& Options = std::set<std::string>());

        void SolveTasks();
    };
}
