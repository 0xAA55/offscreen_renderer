#include <fstream>
#include <sstream>

#include "glsstorage.hpp"

namespace GLObjects
{

    LoadComputeStorageError::LoadComputeStorageError(std::string what) noexcept :
        std::runtime_error(what)
    {
    }

    std::vector<uint8_t> LoadShaderStorageFromFile(std::string FilePath)
    {
        std::ifstream ifs(FilePath, std::ios::binary);
        if (ifs.fail())
        {
            std::stringstream sserr;
            sserr << "Could not open `" << FilePath << "`.";
            throw LoadComputeStorageError(sserr.str());
        }
        return std::vector<uint8_t>(std::istreambuf_iterator<char>(ifs), {});
    }

    ShaderStorage::ShaderStorage(Context& gl, const void* Data, size_t Size) :
        Buffer(gl, Data, Size, gl.DYNAMIC_COPY)
    {
    }

    ShaderStorage::ShaderStorage(Context& gl, size_t Size) :
        ShaderStorage(gl, nullptr, Size)
    {
    }

    ShaderStorage::ShaderStorage(Context& gl, std::string LoadFrom) :
        ShaderStorage(gl, LoadShaderStorageFromFile(LoadFrom))
    {
    }

    void ShaderStorage::Save(std::string Path)
    {
        std::ofstream ofs(Path, std::ios::binary);
        auto buf = std::make_unique<uint8_t[]>(Size);
        GetData(buf.get(), Size, 0);
        ofs.write(reinterpret_cast<char*>(buf.get()), Size);
    }
}
