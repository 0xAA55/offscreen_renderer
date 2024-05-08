#include "tasksstorage.hpp"
#include <fstream>
#include <sstream>

namespace RenderTaskSolver
{
	LoadShaderStorageError::LoadShaderStorageError(const std::string& what) noexcept:
		std::runtime_error(what)
	{
	}

	std::vector<uint8_t> LoadShaderStorageFromFile(const std::string& FilePath)
	{
		std::ifstream ifs(FilePath, std::ios::binary);
		if (ifs.fail())
		{
			throw LoadShaderStorageError((std::stringstream() << "Could not open `" << FilePath << "`.").str());
		}
		return std::vector<uint8_t>(std::istreambuf_iterator<char>(ifs), {});
	}

	TaskShaderStorage::TaskShaderStorage(Context& gl, const std::string& Name, size_t Size) :
		ShaderStorageBuffer(gl, nullptr, Size, gl.DYNAMIC_COPY),
		Name(Name),
		DontKeep(false)
	{
	}

	TaskShaderStorage::TaskShaderStorage(Context& gl, const std::string& Name, const void* Data, size_t Size) :
		ShaderStorageBuffer(gl, Data, Size, gl.DYNAMIC_COPY),
		Name(Name),
		DontKeep(false)
	{
	}

	TaskShaderStorage::TaskShaderStorage(Context& gl, const std::string& Name, const std::string& LoadFrom) :
		ShaderStorageBuffer(gl, LoadShaderStorageFromFile(LoadFrom), gl.DYNAMIC_COPY),
		Name(Name),
		DontKeep(false)
	{
	}

	void TaskShaderStorage::Save()
	{
		std::ofstream ofs(SavePath, std::ios::binary);
		auto buf = std::make_unique<uint8_t[]>(Size);
		GetData(buf.get(), Size, 0);
		ofs.write(reinterpret_cast<char*>(buf.get()), Size);
	}
}
