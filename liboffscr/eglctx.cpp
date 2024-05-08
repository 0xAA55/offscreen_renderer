#include "eglctx.hpp"

#include <KHR/khrplatform.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace RenderTaskSolver
{
	EGLCreateContextError::EGLCreateContextError(const std::string& what) noexcept :
		std::runtime_error(what)
	{
	}

	static void* APIENTRY glGetProcAddress(const char* procname)
	{
		return eglGetProcAddress(procname);
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
		static const EGLint pbufferAttribs[] = {
			EGL_WIDTH, 640,
			EGL_HEIGHT, 480,
			EGL_NONE,
		};
		auto eglQueryDevicesEXT = (PFNEGLQUERYDEVICESEXTPROC)glGetProcAddress("eglQueryDevicesEXT");
		if (!eglQueryDevicesEXT) throw EGLCreateContextError("ERROR: extension eglQueryDevicesEXT not available");

		auto eglGetPlatformDisplayEXT = (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
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
