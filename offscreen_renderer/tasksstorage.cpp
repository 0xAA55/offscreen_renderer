#include "tasksstorage.hpp"
#include <fstream>
#include <sstream>

namespace RenderTaskSolver
{
	LoadShaderStorageError::LoadShaderStorageError(std::string what) noexcept:
		std::runtime_error(what)
	{
	}

	std::vector<uint8_t> LoadShaderStorageFromFile(std::string FilePath)
	{
		std::ifstream ifs(FilePath, std::ios::binary);
		if (ifs.fail())
		{
			throw LoadShaderStorageError((std::stringstream() << "Could not open `" << FilePath << "`.").str());
		}
		return std::vector<uint8_t>(std::istreambuf_iterator<char>(ifs), {});
	}

	TaskShaderStorage::TaskShaderStorage(Context& gl, size_t Size) :
		ShaderStorageBuffer(gl, nullptr, Size, gl.DYNAMIC_COPY)
	{
	}

	TaskShaderStorage::TaskShaderStorage(Context& gl, const void* Data, size_t Size) :
		ShaderStorageBuffer(gl, Data, Size, gl.DYNAMIC_COPY)
	{
	}

	TaskShaderStorage::TaskShaderStorage(Context& gl, std::string LoadFrom) :
		ShaderStorageBuffer(gl, LoadShaderStorageFromFile(LoadFrom), gl.DYNAMIC_COPY)
	{
	}

	void TaskShaderStorage::Save(std::string Path)
	{
		std::ofstream ofs(Path, std::ios::binary);
		auto buf = std::make_unique<uint8_t[]>(Size);
		GetData(buf.get(), Size, 0);
		ofs.write(reinterpret_cast<char*>(buf.get()), Size);
	}
}
