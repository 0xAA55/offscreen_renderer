#include "glbuffer.hpp"

namespace GLObjects
{
	template<GLenum Target>
	Buffer<Target>::Buffer(Context& gl, GLsizeiptr Size, GLenum Usage) :
		Buffer(gl, nullptr, Size, Usage)
	{
	}

	template<GLenum Target>
	Buffer<Target>::Buffer(Context& gl, const void* Data, GLsizeiptr Size, GLenum Usage) :
		gl(gl),
		Size(Size)
	{
		gl.GenBuffers(1, &BO);
		BufferBind<Target> b(gl, BO);
		gl.BufferData(Target, Size, Data, Usage);
	}

	template<GLenum Target>
	Buffer<Target>::~Buffer()
	{
		gl.DeleteBuffers(1, &BO);
	}

	template<GLenum Target>
	void Buffer<Target>::GetData(void* Buffer, GLsizeiptr BufferSize, GLsizeiptr Offset)
	{
		BufferBind<Target> b(gl, BO);
		gl.GetBufferSubData(Target, Offset, BufferSize, Buffer);
	}

	template<GLenum Target>
	void Buffer<Target>::SetData(const void* Data, GLsizeiptr Size, GLsizeiptr Offset)
	{
		BufferBind<Target> b(gl, BO);
		gl.BufferSubData(Target, Offset, Size, Data);
	}

	template Buffer<Context::ARRAY_BUFFER>;
	template Buffer<Context::ATOMIC_COUNTER_BUFFER>;
	template Buffer<Context::COPY_READ_BUFFER>;
	template Buffer<Context::COPY_WRITE_BUFFER>;
	template Buffer<Context::DISPATCH_INDIRECT_BUFFER>;
	template Buffer<Context::DRAW_INDIRECT_BUFFER>;
	template Buffer<Context::ELEMENT_ARRAY_BUFFER>;
	template Buffer<Context::PIXEL_PACK_BUFFER>;
	template Buffer<Context::PIXEL_UNPACK_BUFFER>;
	template Buffer<Context::QUERY_BUFFER>;
	template Buffer<Context::SHADER_STORAGE_BUFFER>;
	template Buffer<Context::TEXTURE_BUFFER>;
	template Buffer<Context::TRANSFORM_FEEDBACK_BUFFER>;
	template Buffer<Context::UNIFORM_BUFFER>;
}
