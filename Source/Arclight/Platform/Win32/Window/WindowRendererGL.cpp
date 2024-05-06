/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 WindowRendererGL.cpp
 */

#include "Window/WindowRendererGL.hpp"
#include "Window/Window.hpp"
#include "Window/WindowHandle.hpp"
#include "Common/Assert.hpp"

#include <GL/glew.h>


#define WGL_TRANSPARENT_ARB 0x200A
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0X2092
#define WGL_CONTEXT_FLAGS_ARB 0X2094
#define WGL_CONTEXT_COREPROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

using WGLCreateContextAttribsFunction	= HGLRC (WINAPI*)(HDC hDC, HGLRC hShareContext, const int* attribList);
using WGLGetExtensionsStringFunction	= const char* (WINAPI*)();
using WGLSwapIntervalFunction			= BOOL (WINAPI*)(int);
using WGLGetSwapIntervalFunction		= int (WINAPI*)();

class WindowRendererGLHandle {

public:

	HDC hdc;
	HGLRC hglrc;
	WGLSwapIntervalFunction wglSwapIntervalEXT;
	WGLGetSwapIntervalFunction wglGetSwapIntervalEXT;

};


template<class T>
static T getWGLFunction(const char* name) {
	return reinterpret_cast<T>(wglGetProcAddress(name));
}



WindowRendererGL::WindowRendererGL() = default;
WindowRendererGL::~WindowRendererGL() = default;

bool WindowRendererGL::create(Window& window) {

	arc_assert(window.isOpen(), "Tried to create OpenGL renderer for non-existing window");

	auto windowHandle = window.getInternalHandle().lock();

	if (!windowHandle) {
		LogE("WindowRendererGL") << "Failed to obtain window handle";
		return false;
	}

	HWND hwnd = windowHandle->getHWND();
	HDC hdc = GetDC(hwnd);

	if (!hdc) {
		LogE("WindowRendererGL") << "Failed to acquire window device context";
		return false;
	}

	DWORD pfFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER | PFD_SUPPORT_COMPOSITION;
	/*if (window.flags() & Window::Flags::SeeThrough) {
		pfFlags |= PFD_SUPPORT_COMPOSITION;
	}*/

	PIXELFORMATDESCRIPTOR pfd {};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = pfFlags;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 8;

	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);

	HGLRC tempRC = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempRC);

	auto wglCreateContextAttribsARB = getWGLFunction<WGLCreateContextAttribsFunction>("wglCreateContextAttribsARB");

	const int attribList[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB,
		WGL_CONTEXT_COREPROFILE_BIT_ARB, 0,
		WGL_TRANSPARENT_ARB, TRUE,
	};

	HGLRC hglrc = wglCreateContextAttribsARB(hdc, nullptr, attribList);
	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(tempRC);
	wglMakeCurrent(hdc, hglrc);

	if (glewInit() != GLEW_OK) {
		LogE("WindowRendererGL") << "OpenGL initialization failed";
		return false;
	}

	LogI("WindowRendererGL") << "Loaded OpenGL " << reinterpret_cast<const char*>(glGetString(GL_VERSION));

	handle = std::make_unique<WindowRendererGLHandle>();
	handle->hdc = hdc;
	handle->hglrc = hglrc;

	std::string extensions = getWGLFunction<WGLGetExtensionsStringFunction>("wglGetExtensionsStringEXT")();
	std::string swapControlString = "WGL_EXT_swap_control";

	bool vSyncSupported = !std::ranges::search(extensions, swapControlString).empty();

	if (vSyncSupported) {

		handle->wglSwapIntervalEXT = getWGLFunction<WGLSwapIntervalFunction>("wglSwapIntervalEXT");
		handle->wglGetSwapIntervalEXT = getWGLFunction<WGLGetSwapIntervalFunction>("wglGetSwapIntervalEXT");

	} else {

		handle->wglSwapIntervalEXT = nullptr;
		handle->wglGetSwapIntervalEXT = nullptr;
		LogW("WindowRendererGL") << "V-Sync not supported, proceeding without it";

	}

	this->window = windowHandle;

	return true;

}

void WindowRendererGL::destroy() {

	auto windowHandle = window.lock();

	if (!windowHandle) {
		LogE("WindowRendererGL") << "Failed to obtain window handle";
		return;
	}

	HWND hwnd = windowHandle->getHWND();

	ReleaseDC(hwnd, handle->hdc);

	handle.reset();
	window.reset();

}

void WindowRendererGL::makeCurrent() {
	wglMakeCurrent(handle->hdc, handle->hglrc);
}

void WindowRendererGL::clearCurrent() {
	wglMakeCurrent(nullptr, nullptr);
}

void WindowRendererGL::swapBuffers() {
	SwapBuffers(handle->hdc);
}

void WindowRendererGL::swapInterval(int interval) {

	if (handle->wglSwapIntervalEXT) {
		handle->wglSwapIntervalEXT(interval);
	}

}

int WindowRendererGL::swapInterval() {

	if (handle->wglSwapIntervalEXT) {
		return handle->wglGetSwapIntervalEXT();
	}

	return 0;

}
