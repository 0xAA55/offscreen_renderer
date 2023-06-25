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
			Buffer(gl, Target, Data.data(), Data.size(), Usage)
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

	extern template Buffer<Context::ARRAY_BUFFER>;
	extern template Buffer<Context::ATOMIC_COUNTER_BUFFER>;
	extern template Buffer<Context::COPY_READ_BUFFER>;
	extern template Buffer<Context::COPY_WRITE_BUFFER>;
	extern template Buffer<Context::DISPATCH_INDIRECT_BUFFER>;
	extern template Buffer<Context::DRAW_INDIRECT_BUFFER>;
	extern template Buffer<Context::ELEMENT_ARRAY_BUFFER>;
	extern template Buffer<Context::PIXEL_PACK_BUFFER>;
	extern template Buffer<Context::PIXEL_UNPACK_BUFFER>;
	extern template Buffer<Context::QUERY_BUFFER>;
	extern template Buffer<Context::SHADER_STORAGE_BUFFER>;
	extern template Buffer<Context::TEXTURE_BUFFER>;
	extern template Buffer<Context::TRANSFORM_FEEDBACK_BUFFER>;
	extern template Buffer<Context::UNIFORM_BUFFER>;

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
