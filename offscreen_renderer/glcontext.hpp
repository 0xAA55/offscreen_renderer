#pragma once

#include <memory>
#include <unordered_map>
#include <type_traits>

#include "glcore.hpp"

namespace GLObjects
{
    using namespace GL;

    class Context;

    template<GLenum BufferType>
    class BufferBind
    {
    protected:
        GLuint BO;
        // GLuint PrevBO;
        Context& gl;

        GLuint* GetPrevBindingBufferVar() const;

    public:
        BufferBind() = delete;
        BufferBind(Context& gl, GLuint BO);
        ~BufferBind();
    };

    class Context : public Version46
    {
    protected:
        using ArrayBufferBind = BufferBind<ARRAY_BUFFER>;
        using AtomicCounterBufferBind = BufferBind<ATOMIC_COUNTER_BUFFER>;
        using CopyReadBufferBind = BufferBind<COPY_READ_BUFFER>;
        using CopyWriteBufferBind = BufferBind<COPY_WRITE_BUFFER>;
        using DispatchIndirectBufferBind = BufferBind<DISPATCH_INDIRECT_BUFFER>;
        using DrawIndirectBufferBind = BufferBind<DRAW_INDIRECT_BUFFER>;
        using ElementArrayBufferBind = BufferBind<ELEMENT_ARRAY_BUFFER>;
        using PixelPackBufferBind = BufferBind<PIXEL_PACK_BUFFER>;
        using PixelUnpackBufferBind = BufferBind<PIXEL_UNPACK_BUFFER>;
        using QueryBufferBind = BufferBind<QUERY_BUFFER>;
        using ShaderStorageBufferBind = BufferBind<SHADER_STORAGE_BUFFER>;
        using TextureBufferBind = BufferBind<TEXTURE_BUFFER>;
        using TransformFeedbackBufferBind = BufferBind<TRANSFORM_FEEDBACK_BUFFER>;
        using UniformBufferBind = BufferBind<UNIFORM_BUFFER>;

        friend class ArrayBufferBind;
        friend class AtomicCounterBufferBind;
        friend class CopyReadBufferBind;
        friend class CopyWriteBufferBind;
        friend class DispatchIndirectBufferBind;
        friend class DrawIndirectBufferBind;
        friend class ElementArrayBufferBind;
        friend class PixelPackBufferBind;
        friend class PixelUnpackBufferBind;
        friend class QueryBufferBind;
        friend class ShaderStorageBufferBind;
        friend class TextureBufferBind;
        friend class TransformFeedbackBufferBind;
        friend class UniformBufferBind;

        GLuint CurBindingArrayBuffer = 0;
        GLuint CurBindingAtomicCounterBuffer = 0;
        GLuint CurBindingCopyReadBuffer = 0;
        GLuint CurBindingCopyWriteBuffer = 0;
        GLuint CurBindingDispatchIndirectBuffer = 0;
        GLuint CurBindingDrawIndirectBuffer = 0;
        GLuint CurBindingElementArrayBuffer = 0;
        GLuint CurBindingPixelPackBuffer = 0;
        GLuint CurBindingPixelUnpackBuffer = 0;
        GLuint CurBindingQueryBuffer = 0;
        GLuint CurBindingShaderStorageBuffer = 0;
        GLuint CurBindingTextureBuffer = 0;
        GLuint CurBindingTransformFeedbackBuffer = 0;
        GLuint CurBindingUniformBuffer = 0;

    public:
        Context() = delete;
        Context(Func_GetProcAddress GetProcAddress);
    };

    extern template Context::ArrayBufferBind;
    extern template Context::AtomicCounterBufferBind;
    extern template Context::CopyReadBufferBind;
    extern template Context::CopyWriteBufferBind;
    extern template Context::DispatchIndirectBufferBind;
    extern template Context::DrawIndirectBufferBind;
    extern template Context::ElementArrayBufferBind;
    extern template Context::PixelPackBufferBind;
    extern template Context::PixelUnpackBufferBind;
    extern template Context::QueryBufferBind;
    extern template Context::ShaderStorageBufferBind;
    extern template Context::TextureBufferBind;
    extern template Context::TransformFeedbackBufferBind;
    extern template Context::UniformBufferBind;
}