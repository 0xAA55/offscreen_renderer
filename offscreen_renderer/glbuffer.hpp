#pragma once

#include<vector>

#include "glcontext.hpp"

namespace GLObjects
{
	template<GLenum Target, typename T> class BufferMapper;
	template<GLenum Target>
	class Buffer
	{
	protected:
		Context& gl;
		GLsizeiptr Size;
		GLuint BO;

	public:
		Buffer() = delete;
		Buffer(const Buffer& Dup) = delete;
		Buffer(Context& gl, GLsizeiptr Size, GLenum Usage);
		Buffer(Context& gl, const void* Data, GLsizeiptr Size, GLenum Usage);
		template<typename VectorType>
		Buffer(Context& gl, const std::vector<VectorType>& Data, GLenum Usage):
			Buffer(gl, Data.data(), Data.size() * sizeof(VectorType), Usage)
		{
		}
		~Buffer();

		inline operator GLuint() const { return BO; }
		inline GLsizeiptr GetSize() const { return Size; }

		void GetData(void* Buffer, GLsizeiptr BufferSize, GLsizeiptr Offset = 0);
		void SetData(const void* Data, GLsizeiptr Size, GLsizeiptr Offset = 0);

		template<typename T>
		BufferMapper<Target, T> MapData(GLintptr Offset, GLsizeiptr Length, GLenum Access)
		{
			return BufferMapper<Target, T>(gl, Offset, Length, Access);
		}
	};

	using ArrayBuffer = Buffer<Context::ARRAY_BUFFER>;
	using AtomicCounterBuffer = Buffer<Context::ATOMIC_COUNTER_BUFFER>;
	using CopyReadBuffer = Buffer<Context::COPY_READ_BUFFER>;
	using CopyWriteBuffer = Buffer<Context::COPY_WRITE_BUFFER>;
	using DispatchIndirectBuffer = Buffer<Context::DISPATCH_INDIRECT_BUFFER>;
	using DrawIndirectBuffer = Buffer<Context::DRAW_INDIRECT_BUFFER>;
	using ElementArrayBuffer = Buffer<Context::ELEMENT_ARRAY_BUFFER>;
	using PixelPackBuffer = Buffer<Context::PIXEL_PACK_BUFFER>;
	using PixelUnpackBuffer = Buffer<Context::PIXEL_UNPACK_BUFFER>;
	using QueryBuffer = Buffer<Context::QUERY_BUFFER>;
	using ShaderStorageBuffer = Buffer<Context::SHADER_STORAGE_BUFFER>;
	using TextureBuffer = Buffer<Context::TEXTURE_BUFFER>;
	using TransformFeedbackBuffer = Buffer<Context::TRANSFORM_FEEDBACK_BUFFER>;
	using UniformBuffer = Buffer<Context::UNIFORM_BUFFER>;

	extern template ArrayBuffer;
	extern template AtomicCounterBuffer;
	extern template CopyReadBuffer;
	extern template CopyWriteBuffer;
	extern template DispatchIndirectBuffer;
	extern template DrawIndirectBuffer;
	extern template ElementArrayBuffer;
	extern template PixelPackBuffer;
	extern template PixelUnpackBuffer;
	extern template QueryBuffer;
	extern template ShaderStorageBuffer;
	extern template TextureBuffer;
	extern template TransformFeedbackBuffer;
	extern template UniformBuffer;

	template<GLenum BufferType>
	class BufferBind
	{
	protected:
		GLuint BO;
		Context& gl;

	public:
		BufferBind() = delete;
		BufferBind(Context& gl, GLuint BO);
		~BufferBind();
	};

	using ArrayBufferBind = Buffer<Context::ARRAY_BUFFER>;
	using AtomicCounterBufferBind = Buffer<Context::ATOMIC_COUNTER_BUFFER>;
	using CopyReadBufferBind = Buffer<Context::COPY_READ_BUFFER>;
	using CopyWriteBufferBind = Buffer<Context::COPY_WRITE_BUFFER>;
	using DispatchIndirectBufferBind = Buffer<Context::DISPATCH_INDIRECT_BUFFER>;
	using DrawIndirectBufferBind = Buffer<Context::DRAW_INDIRECT_BUFFER>;
	using ElementArrayBufferBind = Buffer<Context::ELEMENT_ARRAY_BUFFER>;
	using PixelPackBufferBind = Buffer<Context::PIXEL_PACK_BUFFER>;
	using PixelUnpackBufferBind = Buffer<Context::PIXEL_UNPACK_BUFFER>;
	using QueryBufferBind = Buffer<Context::QUERY_BUFFER>;
	using ShaderStorageBufferBind = Buffer<Context::SHADER_STORAGE_BUFFER>;
	using TextureBufferBind = Buffer<Context::TEXTURE_BUFFER>;
	using TransformFeedbackBufferBind = Buffer<Context::TRANSFORM_FEEDBACK_BUFFER>;
	using UniformBufferBind = Buffer<Context::UNIFORM_BUFFER>;

	extern template ArrayBufferBind;
	extern template AtomicCounterBufferBind;
	extern template CopyReadBufferBind;
	extern template CopyWriteBufferBind;
	extern template DispatchIndirectBufferBind;
	extern template DrawIndirectBufferBind;
	extern template ElementArrayBufferBind;
	extern template PixelPackBufferBind;
	extern template PixelUnpackBufferBind;
	extern template QueryBufferBind;
	extern template ShaderStorageBufferBind;
	extern template TextureBufferBind;
	extern template TransformFeedbackBufferBind;
	extern template UniformBufferBind;

	template<GLenum Target, typename T>
	class BufferMapper
	{
	protected:
		friend class Buffer<Target>;

		Context& gl;
		BufferBind<Target> Binding;
		T* Pointer;

		BufferMapper(Buffer<Target>& B, GLintptr Offset, GLsizeiptr Length, GLenum Access) :
			gl(B.gl),
			Binding(gl, B)
		{
			Pointer = static_cast<T*>(gl.MapBufferRange(Target, Offset, Length, Access));
		}

	public:
		BufferMapper() = delete;
		~BufferMapper()
		{
			gl.UnmapBuffer(Target);
			Pointer = nullptr;
		}

		operator T* () const { return Pointer; }
	};
}
