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
	BufferBind<Target>::BufferBind(Context& gl, GLuint BO) :
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

	template ArrayBuffer;
	template AtomicCounterBuffer;
	template CopyReadBuffer;
	template CopyWriteBuffer;
	template DispatchIndirectBuffer;
	template DrawIndirectBuffer;
	template ElementArrayBuffer;
	template PixelPackBuffer;
	template PixelUnpackBuffer;
	template QueryBuffer;
	template ShaderStorageBuffer;
	template TextureBuffer;
	template TransformFeedbackBuffer;
	template UniformBuffer;

	template ArrayBufferBind;
	template AtomicCounterBufferBind;
	template CopyReadBufferBind;
	template CopyWriteBufferBind;
	template DispatchIndirectBufferBind;
	template DrawIndirectBufferBind;
	template ElementArrayBufferBind;
	template PixelPackBufferBind;
	template PixelUnpackBufferBind;
	template QueryBufferBind;
	template ShaderStorageBufferBind;
	template TextureBufferBind;
	template TransformFeedbackBufferBind;
	template UniformBufferBind;
}
