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
			EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_DEPTH_SIZE, 8,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
			EGL_NONE
		};
		auto eglQueryDevicesEXT = reinterpret_cast<PFNEGLQUERYDEVICESEXTPROC>(glGetProcAddress("eglQueryDevicesEXT"));
		if (!eglQueryDevicesEXT) throw EGLCreateContextError("ERROR: extension eglQueryDevicesEXT not available");

		auto eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
		if (!eglGetPlatformDisplayEXT) throw EGLCreateContextError("ERROR: extension eglGetPlatformDisplayEXT not available");

		static const int MAX_DEVICES = 16;
		EGLDeviceEXT devices[MAX_DEVICES];
		EGLint numDevices;

		eglQueryDevicesEXT(MAX_DEVICES, devices, &numDevices);

		EGLDisplay eglDpy = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, devices[0], 0);

		EGLint major, minor;

		eglInitialize(eglDpy, &major, &minor);

		// 2. Select appropriate configuration
		EGLint numConfigs;
		EGLConfig eglCfg;

		eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

		// 3. Bind the API 
		eglBindAPI(EGL_OPENGL_API);

		// 4. create the context
		EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, NULL);

		eglMakeCurrent(eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, eglCtx);
		
		glCtx = std::make_unique<Context>(glGetProcAddress);
	}

	const Context& EGLCtx::GetOpenGLContext() const
	{
		return *glCtx;
	}
}
