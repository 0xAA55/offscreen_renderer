#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "glcontext.hpp"
#include "glbuffer.hpp"

namespace GLObjects
{
    class LoadComputeStorageError : public std::runtime_error
    {
    public:
        LoadComputeStorageError(std::string what) noexcept;
    };

    class ShaderStorage : public Buffer<Context::SHADER_STORAGE_BUFFER>
    {
    protected:

    public:
        ShaderStorage(Context& gl, size_t Size);
        ShaderStorage(Context& gl, const void* Data, size_t Size);
        ShaderStorage(Context& gl, std::string LoadFrom);

        template<typename UnitType>
        ShaderStorage(Context& gl, const std::vector<UnitType>& Data) :
            ShaderStorage(gl, Data.data(), Data.size() * sizeof(UnitType))
        {
        }

        void Save(std::string Path);
    };
}
