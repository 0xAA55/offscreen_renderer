#include "glbuffer.hpp"

namespace GLObjects
{
	template<GLenum Target>
	Buffer<Target>::Buffer(const Context& gl, GLsizeiptr Size, GLenum Usage) :
		Buffer(gl, nullptr, Size, Usage)
	{
	}

	template<GLenum Target>
	Buffer<Target>::Buffer(const Context& gl, const void* Data, GLsizeiptr Size, GLenum Usage) :
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
	BufferBind<Target>::BufferBind(const Context& gl, GLuint BO) :
		gl(gl),
		BO(BO)
	{
		gl.BindBuffer(Target, BO);
	}

	template<GLenum Target>
	BufferBind<Target>::~BufferBind()
	{
		gl.BindBuffer(Target, 0);
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

	template class Buffer<Context::ARRAY_BUFFER>;
	template class Buffer<Context::ATOMIC_COUNTER_BUFFER>;
	template class Buffer<Context::COPY_READ_BUFFER>;
	template class Buffer<Context::COPY_WRITE_BUFFER>;
	template class Buffer<Context::DISPATCH_INDIRECT_BUFFER>;
	template class Buffer<Context::DRAW_INDIRECT_BUFFER>;
	template class Buffer<Context::ELEMENT_ARRAY_BUFFER>;
	template class Buffer<Context::PIXEL_PACK_BUFFER>;
	template class Buffer<Context::PIXEL_UNPACK_BUFFER>;
	template class Buffer<Context::QUERY_BUFFER>;
	template class Buffer<Context::SHADER_STORAGE_BUFFER>;
	template class Buffer<Context::TEXTURE_BUFFER>;
	template class Buffer<Context::TRANSFORM_FEEDBACK_BUFFER>;
	template class Buffer<Context::UNIFORM_BUFFER>;

	template class BufferBind<Context::ARRAY_BUFFER>;
	template class BufferBind<Context::ATOMIC_COUNTER_BUFFER>;
	template class BufferBind<Context::COPY_READ_BUFFER>;
	template class BufferBind<Context::COPY_WRITE_BUFFER>;
	template class BufferBind<Context::DISPATCH_INDIRECT_BUFFER>;
	template class BufferBind<Context::DRAW_INDIRECT_BUFFER>;
	template class BufferBind<Context::ELEMENT_ARRAY_BUFFER>;
	template class BufferBind<Context::PIXEL_PACK_BUFFER>;
	template class BufferBind<Context::PIXEL_UNPACK_BUFFER>;
	template class BufferBind<Context::QUERY_BUFFER>;
	template class BufferBind<Context::SHADER_STORAGE_BUFFER>;
	template class BufferBind<Context::TEXTURE_BUFFER>;
	template class BufferBind<Context::TRANSFORM_FEEDBACK_BUFFER>;
	template class BufferBind<Context::UNIFORM_BUFFER>;
}
