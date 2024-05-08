#pragma once

#include "glsstorage.hpp"

namespace RenderTaskSolver
{
    using namespace GLObjects;

    class TaskShaderStorage : public ShaderStorage
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
    };
}

