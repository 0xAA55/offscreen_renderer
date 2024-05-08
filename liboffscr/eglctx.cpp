#include "eglctx.hpp"

#undef APIENTRY

#include <KHR/khrplatform.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

#ifndef APIENTRY
#  if defined(__MINGW32__) || defined(__CYGWIN__) || (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
#    define APIENTRY __stdcall
#  else
#    define APIENTRY
#  endif
#endif


namespace RenderTaskSolver
{
	EGLCreateContextError::EGLCreateContextError(const std::string& what) noexcept :
		std::runtime_error(what)
	{
	}

	static void* APIENTRY glGetProcAddress(const char* procname)
	{
		return reinterpret_cast<void*>(eglGetProcAddress(procname));
	}

	EGLCtx::EGLCtx()
	{
		static const EGLint configAttribs[] = {
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_DEPTH_SIZE, 24,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
			EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
			EGL_NONE
		};
		static const EGLint pbufferAttribs[] = {
			EGL_WIDTH, 9,
			EGL_HEIGHT, 9,
			EGL_NONE,
		};
		static const EGLint attribList[] = {
			EGL_CONTEXT_MAJOR_VERSION, 3,
			EGL_CONTEXT_MINOR_VERSION, 0,
			EGL_NONE,
		};
		EGLBoolean ret = 0;

		// 1. Initialize EGL
		EGLDisplayPtr = std::make_unique<EGLDisplay>(eglGetDisplay(EGL_DEFAULT_DISPLAY));
		auto& eglDpy = *EGLDisplayPtr;
		if (!eglDpy) throw EGLCreateContextError("eglGetDisplay(EGL_DEFAULT_DISPLAY) returns NULL.");

		EGLint major, minor;
		ret = eglInitialize(eglDpy, &major, &minor);
		if (!ret) throw EGLCreateContextError("eglInitialize() returns GL_FALSE.");

		// 2. Select appropriate configuration
		EGLint numConfigs = 0;
		EGLConfig eglCfg = nullptr;

		ret = eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);
		if (!ret) throw EGLCreateContextError("eglChooseConfig() returns GL_FALSE.");

		// 3. Create a surface
		EGLSurfacePtr = std::make_unique<EGLSurface>(eglCreatePbufferSurface(eglDpy, eglCfg, pbufferAttribs));
		auto& eglSurf = *EGLSurfacePtr;
		if (!eglSurf) throw EGLCreateContextError("eglCreatePbufferSurface() returns NULL.");

		// 4. Bind the API
		ret = eglBindAPI(EGL_OPENGL_API);

		// 5. Create a context and make it current
		EGLContextPtr = std::make_unique<EGLSurface>(eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, attribList));
		auto& eglCtx = *EGLContextPtr;
		if (!eglCtx) throw EGLCreateContextError("eglCreateContext() returns NULL.");

		ret = eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);

		glCtx = std::make_unique<Context>(glGetProcAddress);
	}

	EGLCtx::~EGLCtx()
	{
		eglTerminate(*EGLDisplayPtr);
	}

	const Context& EGLCtx::GetOpenGLContext() const
	{
		return *glCtx;
	}
}
