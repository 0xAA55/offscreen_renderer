#pragma once

#include "glbuffer.hpp"

namespace RenderTaskSolver
{
    using namespace GLObjects;

    class LoadShaderStorageError : public std::runtime_error
    {
    public:
        LoadShaderStorageError(const std::string& what) noexcept;
    };

    class TaskShaderStorage : public ShaderStorageBuffer
    {
    protected:
        std::string Name;

    public:
        TaskShaderStorage(Context& gl, const std::string& Name, size_t Size);
        TaskShaderStorage(Context& gl, const std::string& Name, const void* Data, size_t Size);
        TaskShaderStorage(Context& gl, const std::string& Name, const std::string& LoadFrom);

        template<typename UnitType>
        TaskShaderStorage(Context& gl, const std::string& Name, const std::vector<UnitType>& Data) :
            TaskShaderStorage(gl, Name, Data.data(), Data.size() * sizeof(UnitType))
        {
        }

        bool DontKeep;
        std::string SavePath;

        inline const std::string& GetName() const { return Name; }

        void Save();
    };
}

