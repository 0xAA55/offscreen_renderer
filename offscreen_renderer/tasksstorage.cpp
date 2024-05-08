#include "tasksstorage.hpp"

namespace RenderTaskSolver
{
	TaskShaderStorage::TaskShaderStorage(Context& gl, size_t Size) :
		ShaderStorage(gl, Size)
	{
	}

	TaskShaderStorage::TaskShaderStorage(Context& gl, const void* Data, size_t Size) :
		ShaderStorage(gl, Data, Size)
	{
	}

	TaskShaderStorage::TaskShaderStorage(Context& gl, std::string LoadFrom) :
		ShaderStorage(gl, LoadFrom)
	{
	}
}
