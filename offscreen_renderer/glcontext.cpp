#include "glcontext.hpp"

namespace GLObjects
{
    Context::Context(Func_GetProcAddress GetProcAddress) :
        Version46(GetProcAddress)
    {
    }

    template<GLenum Target>
    GLuint* BufferBind<Target>::GetPrevBindingBufferVar() const
    {
        switch (Target)
        {
        case gl.ARRAY_BUFFER: return &gl.CurBindingArrayBuffer;
        case gl.ATOMIC_COUNTER_BUFFER: return &gl.CurBindingAtomicCounterBuffer;
        case gl.COPY_READ_BUFFER: return &gl.CurBindingCopyReadBuffer;
        case gl.COPY_WRITE_BUFFER: return &gl.CurBindingCopyWriteBuffer;
        case gl.DISPATCH_INDIRECT_BUFFER: return &gl.CurBindingDispatchIndirectBuffer;
        case gl.DRAW_INDIRECT_BUFFER: return &gl.CurBindingDrawIndirectBuffer;
        case gl.ELEMENT_ARRAY_BUFFER: return &gl.CurBindingElementArrayBuffer;
        case gl.PIXEL_PACK_BUFFER: return &gl.CurBindingPixelPackBuffer;
        case gl.PIXEL_UNPACK_BUFFER: return &gl.CurBindingPixelUnpackBuffer;
        case gl.QUERY_BUFFER: return &gl.CurBindingQueryBuffer;
        case gl.SHADER_STORAGE_BUFFER: return &gl.CurBindingShaderStorageBuffer;
        case gl.TEXTURE_BUFFER: return &gl.CurBindingTextureBuffer;
        case gl.TRANSFORM_FEEDBACK_BUFFER: return &gl.CurBindingTransformFeedbackBuffer;
        case gl.UNIFORM_BUFFER: return &gl.CurBindingUniformBuffer;
        }
        return nullptr;
    }

    template<GLenum Target>
    BufferBind<Target>::BufferBind(Context& gl, GLuint BO) :
        gl(gl),
        BO(BO)
    {
        /*
        GLuint& Prev = *GetPrevBindingBufferVar();
        PrevBO = Prev;
        if (PrevBO != BO)
        {
            gl.BindBuffer(Target, BO);
            Prev = BO;
        }
        */
        gl.BindBuffer(Target, BO);
    }

    template<GLenum Target>
    BufferBind<Target>::~BufferBind()
    {
        /*GLuint& Prev = *GetPrevBindingBufferVar();
        if (PrevBO != BO)
        {
            gl.BindBuffer(Target, PrevBO);
            Prev = PrevBO;
        }
        */
        gl.BindBuffer(Target, 0);
    }

    template Context::ArrayBufferBind;
    template Context::AtomicCounterBufferBind;
    template Context::CopyReadBufferBind;
    template Context::CopyWriteBufferBind;
    template Context::DispatchIndirectBufferBind;
    template Context::DrawIndirectBufferBind;
    template Context::ElementArrayBufferBind;
    template Context::PixelPackBufferBind;
    template Context::PixelUnpackBufferBind;
    template Context::QueryBufferBind;
    template Context::ShaderStorageBufferBind;
    template Context::TextureBufferBind;
    template Context::TransformFeedbackBufferBind;
    template Context::UniformBufferBind;
}
