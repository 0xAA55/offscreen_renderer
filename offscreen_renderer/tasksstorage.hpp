#pragma once

#include "glbuffer.hpp"

namespace RenderTaskSolver
{
    using namespace GLObjects;

    class LoadShaderStorageError : public std::runtime_error
    {
    public:
        LoadShaderStorageError(std::string what) noexcept;
    };

    class TaskShaderStorage : public ShaderStorageBuffer
    {
    protected:

    public:
        TaskShaderStorage(Context& gl, size_t Size);
        TaskShaderStorage(Context& gl, const void* Data, size_t Size);
        TaskShaderStorage(Context& gl, std::string LoadFrom);

        template<typename UnitType>
        TaskShaderStorage(Context& gl, const std::vector<UnitType>& Data) :
            TaskShaderStorage(gl, Data.data(), Data.size() * sizeof(UnitType))
        {
        }

        std::string SavePath;
        void Save(std::string Path);
    };
}

