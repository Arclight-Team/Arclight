/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 window.cpp
 */

#include "window.hpp"
#include "windowhandle.hpp"
#include "input/inputsystem.hpp"
#include "image/image.hpp"
#include "render/gle/glecore.hpp"
#include "util/assert.hpp"
#include "util/log.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>


GLFWmonitor** Window::connectedMonitors = nullptr;
bool Window::monitorsChanged = true;
u32 Window::monitorCount = 0;



Window::Window() : backupWidth(0), backupHeight(0), windowHandle(nullptr),
	moveFunction(nullptr), resizeFunction(nullptr), stateChangeFunction(nullptr), fbResizeFunction(nullptr), dropFunction(nullptr), cursor(*this) {

	initMonitorCallback();

}



Window::~Window() {

	if (isOpen()) {
		close();
	}

}



void Window::setWindowConfig(const WindowConfig& config) {

	if (isOpen()) {
		Log::warn("Window", "Window config will have no effect until re-opening the window");
		return;
	}

	glfwWindowHint(GLFW_RESIZABLE, config.resizable);
	glfwWindowHint(GLFW_MAXIMIZED, config.maximized);
	glfwWindowHint(GLFW_FLOATING, config.alwaysOnTop);

	glfwWindowHint(GLFW_RED_BITS, config.redBits);
	glfwWindowHint(GLFW_GREEN_BITS, config.greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, config.blueBits);
	glfwWindowHint(GLFW_ALPHA_BITS, config.alphaBits);
	glfwWindowHint(GLFW_DEPTH_BITS, config.depthBits);
	glfwWindowHint(GLFW_STENCIL_BITS, config.stencilBits);
	glfwWindowHint(GLFW_SAMPLES, config.samples);
	glfwWindowHint(GLFW_SRGB_CAPABLE, config.srgbRendering);

#ifdef GLE_DISABLE_DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, false);
#else
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, config.debugContext);
#endif

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, config.forwardContext);

	switch (config.profile) {

		case WindowConfig::OpenGLProfile::Core:
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			break;

		case WindowConfig::OpenGLProfile::Compatibility:
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
			break;

		case WindowConfig::OpenGLProfile::Any:
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
			break;

		default:
			arc_assert(true, "Invalid OpenGL profile requested (%d)", static_cast<u32>(config.profile));
			break;

	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.openglMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.openglMinor);

}



bool Window::create(u32 w, u32 h, const std::string& title) {

	if (isOpen()) {
		Log::warn("Window", "Cannot open window that is already open");
		return true;
	}

	GLFWwindow* handle = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);

	if (handle) {

		windowHandle = std::make_shared<WindowHandle>();
		windowHandle->handle = handle;
		windowHandle->userPtr.window = this;
		glfwSetWindowUserPointer(windowHandle->handle, static_cast<void*>(&windowHandle->userPtr));

		enableContext();

		if (!setupGLContext()) {
			close();
			return false;
		}

	} else {
		Log::error("Window", "Failed to create window");
	}

	return isOpen();

}



bool Window::createFullscreen(const std::string& title, u32 monitorID) {

	if (isOpen()) {
		Log::warn("Window", "Cannot open window that is already open");
		return true;
	}

	if (monitorID >= monitorCount || connectedMonitors[monitorID] == nullptr) {
		Log::error("Window", "No valid monitor with ID=%d found", monitorID);
		return false;
	}

	GLFWmonitor* monitor = connectedMonitors[monitorID];
	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

	arc_assert(videoMode, "Video mode of non-created window on monitor ID=%d null", monitorID);

	contextConfig.redBits = videoMode->redBits;
	contextConfig.greenBits = videoMode->greenBits;
	contextConfig.blueBits = videoMode->blueBits;

	glfwWindowHint(GLFW_RED_BITS, contextConfig.redBits);
	glfwWindowHint(GLFW_GREEN_BITS, contextConfig.greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, contextConfig.blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);

	GLFWwindow* handle = glfwCreateWindow(videoMode->width, videoMode->height, title.c_str(), monitor, nullptr);

	if (handle) {

		windowHandle = std::make_shared<WindowHandle>();
		windowHandle->handle = handle;
		windowHandle->userPtr.window = this;
		glfwSetWindowUserPointer(windowHandle->handle, static_cast<void*>(&windowHandle->userPtr));

		backupWidth = videoMode->width;
		backupHeight = videoMode->height;
		enableContext();

		if (!setupGLContext()) {
			close();
			return false;
		}

	} else {
		Log::error("Window", "Failed to create window");
	}

	return isOpen();

}



void Window::close() {

	if (!isOpen()) {
		Log::warn("Window", "Cannot close window that is not open");
		return;
	}

	InputSystem* input = windowHandle->userPtr.input;

	if (input) {
		input->disconnect();
	}

	cursor.destroyAll();

	glfwDestroyWindow(windowHandle->handle);

	windowHandle.reset();
	backupWidth = 0;
	backupHeight = 0;

}



void Window::setWindowed() {

	arc_assert(isOpen(), "Tried to set windowed mode for non-existing window");

	if (!isFullscreen()) {
		return;
	}

	glfwSetWindowMonitor(windowHandle->handle, nullptr, 40, 40, backupWidth, backupHeight, GLFW_DONT_CARE);

}



void Window::setFullscreen(u32 monitorID) {

	arc_assert(isOpen(), "Tried to set fullscreen mode for non-existing window");

	if (isFullscreen()) {
		return;
	}

	if (monitorID >= monitorCount || connectedMonitors[monitorID] == nullptr) {
		Log::error("Window", "No valid monitor with ID=%d found", monitorID);
		return;
	}

	backupWidth = getWidth();
	backupHeight = getHeight();

	
	GLFWmonitor* monitor = connectedMonitors[monitorID];
	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

	arc_assert(videoMode, "Video mode of non-created window on monitor ID=%d null", monitorID);

	glfwSetWindowMonitor(windowHandle->handle, monitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);

}



void Window::setTitle(const std::string& title) {

	arc_assert(isOpen(), "Tried to set window title for non-existing window");
	glfwSetWindowTitle(windowHandle->handle, title.c_str());

}



void Window::setX(u32 x) {
		
	arc_assert(isOpen(), "Tried to set window x-position for non-existing window");
	glfwSetWindowPos(windowHandle->handle, x, getY());

}



void Window::setY(u32 y) {

	arc_assert(isOpen(), "Tried to set window y-position for non-existing window");
	glfwSetWindowPos(windowHandle->handle, getX(), y);

}



void Window::setPosition(const Vec2ui& pos) {
	setPosition(pos.x, pos.y);
}



void Window::setPosition(u32 x, u32 y) {

	arc_assert(isOpen(), "Tried to set window position for non-existing window");
	glfwSetWindowPos(windowHandle->handle, x, y);

}



void Window::setSize(const Vec2ui& size) {
	setSize(size.x, size.y);
}



void Window::setSize(u32 w, u32 h) {

	arc_assert(isOpen(), "Tried to set window size for non-existing window");
	glfwSetWindowSize(windowHandle->handle, w, h);

}



void Window::setLimits(const Vec2ui& min, const Vec2ui& max) {
	setLimits(min.x, min.y, max.x, max.y);
}



void Window::setLimits(u32 minW, u32 minH, u32 maxW, u32 maxH) {

	arc_assert(isOpen(), "Tried to set window limits for non-existing window");
	glfwSetWindowSizeLimits(windowHandle->handle, minW, minH, maxW, maxH);

}



void Window::setMinLimits(const Vec2ui& min) {
	setMinLimits(min.x, min.y);
}



void Window::setMinLimits(u32 minW, u32 minH) {

	arc_assert(isOpen(), "Tried to set window min limits for non-existing window");
	glfwSetWindowSizeLimits(windowHandle->handle, minW, minH, GLFW_DONT_CARE, GLFW_DONT_CARE);

}



void Window::setMaxLimits(const Vec2ui& max) {
	setMinLimits(max.x, max.y);
}



void Window::setMaxLimits(u32 maxW, u32 maxH) {

	arc_assert(isOpen(), "Tried to set window max limits for non-existing window");
	glfwSetWindowSizeLimits(windowHandle->handle, GLFW_DONT_CARE, GLFW_DONT_CARE, maxW, maxH);

}



void Window::setAspectRatio(double aspect) {

	arc_assert(isOpen(), "Tried to set window aspect ratio for non-existing window");
	glfwSetWindowAspectRatio(windowHandle->handle, aspect * 1000, 1000);

}



void Window::setOpacity(double opacity) {

	arc_assert(isOpen(), "Tried to set window opacity for non-existing window");
	glfwSetWindowOpacity(windowHandle->handle, opacity);

}



void Window::setIcon(const Image<Pixel::RGBA8>& icon) {

	arc_assert(isOpen(), "Tried to set window icon for non-existing window");
	
	//Ugly, but legal here: img is const so the data will never be modified (and shouldn't anyways)
	const GLFWimage img { static_cast<i32>(icon.getWidth()), static_cast<i32>(icon.getHeight()), reinterpret_cast<u8*>(const_cast<PixelRGBA8*>(icon.getImageBuffer().data()))};
	glfwSetWindowIcon(windowHandle->handle, 1, &img);

}



void Window::setAlwaysOnTop(bool onTop) {

	arc_assert(isOpen(), "Tried to set window always-on-top on non-existing window");
	glfwSetWindowAttrib(windowHandle->handle, GLFW_FLOATING, onTop);

}



u32 Window::getWidth() const {

	arc_assert(isOpen(), "Tried to get window width for non-existing window");

	i32 w = 0;
	glfwGetWindowSize(windowHandle->handle, &w, nullptr);
	return w;

}



u32 Window::getHeight() const {

	arc_assert(isOpen(), "Tried to get window height for non-existing window");

	i32 h = 0;
	glfwGetWindowSize(windowHandle->handle, nullptr, &h);
	return h;

}




u32 Window::getFramebufferWidth() const {

	arc_assert(isOpen(), "Tried to get window framebuffer width for non-existing window");

	i32 w = 0;
	glfwGetFramebufferSize(windowHandle->handle, &w, nullptr);
	return w;

}



u32 Window::getFramebufferHeight() const {

	arc_assert(isOpen(), "Tried to get window framebuffer height for non-existing window");

	i32 h = 0;
	glfwGetFramebufferSize(windowHandle->handle, nullptr, &h);
	return h;

}



u32 Window::getX() const {

	arc_assert(isOpen(), "Tried to get window x-position for non-existing window");

	i32 x = 0;
	glfwGetWindowPos(windowHandle->handle, &x, nullptr);
	return x;

}



u32 Window::getY() const {

	arc_assert(isOpen(), "Tried to get window y-position for non-existing window");

	i32 y = 0;
	glfwGetWindowPos(windowHandle->handle, nullptr, &y);
	return y;

}



Vec2ui Window::getPosition() const {

	arc_assert(isOpen(), "Tried to get window position for non-existing window");

	Vec2i v;
	glfwGetWindowPos(windowHandle->handle, &v.x, &v.y);
	return v;

}



void Window::minimize() {

	arc_assert(isOpen(), "Tried to minimize window for non-existing window");
	glfwIconifyWindow(windowHandle->handle);

}



void Window::restore() {

	arc_assert(isOpen(), "Tried to restore window for non-existing window");
	glfwRestoreWindow(windowHandle->handle);

}



void Window::maximize() {

	arc_assert(isOpen(), "Tried to maximize window for non-existing window");
	glfwMaximizeWindow(windowHandle->handle);

}



void Window::show() {

	arc_assert(isOpen(), "Tried to show window for non-existing window");
	glfwShowWindow(windowHandle->handle);

}



void Window::hide() {

	arc_assert(isOpen(), "Tried to hide window for non-existing window");
	glfwHideWindow(windowHandle->handle);

}



void Window::focus() {

	arc_assert(isOpen(), "Tried to focus window for non-existing window");
	glfwFocusWindow(windowHandle->handle);

}



void Window::requestAttention() {

	arc_assert(isOpen(), "Tried to request attention for non-existing window");
	glfwRequestWindowAttention(windowHandle->handle);

}



void Window::disableConstraints() {

	arc_assert(isOpen(), "Tried to disable size constraints for non-existing window");
	glfwSetWindowSizeLimits(windowHandle->handle, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetWindowAspectRatio(windowHandle->handle, GLFW_DONT_CARE, GLFW_DONT_CARE);

}



void Window::pollEvents() {
	glfwPollEvents();
}



void Window::swapBuffers() {

	arc_assert(isOpen(), "Tried to swap buffers for non-existing window");
	glfwSwapBuffers(windowHandle->handle);

}



void Window::enableContext() {

	arc_assert(isOpen(), "Tried to enable OpenGL context for non-existing window");
	glfwMakeContextCurrent(windowHandle->handle);

}



void Window::disableContext() {
	glfwMakeContextCurrent(nullptr);
}



void Window::enableVSync() {
	glfwSwapInterval(1);
}



void Window::disableVSync() {
	glfwSwapInterval(0);
}



void Window::requestClose() {

	arc_assert(isOpen(), "Tried to request close for non-existing window");
	glfwSetWindowShouldClose(windowHandle->handle, true);

}



void Window::dismissCloseRequest() {

	arc_assert(isOpen(), "Tried to dismiss close request for non-existing window");
	glfwSetWindowShouldClose(windowHandle->handle, false);

}



std::string Window::getClipboardString() const {

	arc_assert(isOpen(), "Tried to obtain clipboard string for non-existing window");

	const char* str = glfwGetClipboardString(windowHandle->handle);

	if (str) {
		return str;
	} else {
		return "";
	}

}



void Window::setClipboardString(const std::string& str) {

	arc_assert(isOpen(), "Tried to set clipboard string for non-existing window");
	glfwSetClipboardString(windowHandle->handle, str.c_str());

}


RenderCursor& Window::getRenderCursor() {

	arc_assert(isOpen(), "Tried to obtain cursor for non-existing window");
	return cursor;

}



const RenderCursor& Window::getRenderCursor() const {

	arc_assert(isOpen(), "Tried to obtain cursor for non-existing window");
	return cursor;

}



bool Window::isOpen() const {
	return windowHandle != nullptr;
}



bool Window::isFullscreen() const {

	arc_assert(isOpen(), "Tried to obtain fullscreen state for non-existing window");
	return glfwGetWindowMonitor(windowHandle->handle) != nullptr;

}



bool Window::closeRequested() const {

	arc_assert(isOpen(), "Tried to fetch close request state for non-existing window");
	return glfwWindowShouldClose(windowHandle->handle);

}


bool Window::focused() const {

	arc_assert(isOpen(), "Tried to fetch focus state for non-existing window");
	return glfwGetWindowAttrib(windowHandle->handle, GLFW_FOCUSED);

}



bool Window::minimized() const {

	arc_assert(isOpen(), "Tried to fetch minimized state for non-existing window");
	return glfwGetWindowAttrib(windowHandle->handle, GLFW_ICONIFIED);

}



bool Window::maximized() const {

	arc_assert(isOpen(), "Tried to fetch maximized state for non-existing window");
	return glfwGetWindowAttrib(windowHandle->handle, GLFW_MAXIMIZED);

}



bool Window::hovered() const {

	arc_assert(isOpen(), "Tried to fetch hovered state for non-existing window");
	return glfwGetWindowAttrib(windowHandle->handle, GLFW_HOVERED);

}



bool Window::visible() const {

	arc_assert(isOpen(), "Tried to fetch visibility state for non-existing window");
	return glfwGetWindowAttrib(windowHandle->handle, GLFW_VISIBLE);

}



bool Window::resizable() const {

	arc_assert(isOpen(), "Tried to fetch resizeable state for non-existing window");
	return glfwGetWindowAttrib(windowHandle->handle, GLFW_RESIZABLE);

}



bool Window::decorated() const {

	arc_assert(isOpen(), "Tried to fetch decoration state for non-existing window");
	return glfwGetWindowAttrib(windowHandle->handle, GLFW_DECORATED);

}



bool Window::alwaysOnTop() const {

	arc_assert(isOpen(), "Tried to fetch always-on-top state for non-existing window");
	return glfwGetWindowAttrib(windowHandle->handle, GLFW_FLOATING);

}



u32 Window::getMonitorCount() {
	return monitorCount;
}



Monitor Window::getMonitor(u32 id) {

	arc_assert(id < monitorCount, "Accessing non-existing monitor with ID=%d", id);
	arc_assert(connectedMonitors[id], "Monitor with ID=%d is null", id);

	GLFWmonitor* monitorHandle = connectedMonitors[id];
	i32 vx, vy, wx, wy, ww, wh, pw, ph;

	glfwGetMonitorPos(monitorHandle, &vx, &vy);
	glfwGetMonitorWorkarea(monitorHandle, &wx, &wy, &ww, &wh);
	glfwGetMonitorPhysicalSize(monitorHandle, &pw, &ph);
	const char* name = glfwGetMonitorName(monitorHandle);
	const GLFWvidmode* mode = glfwGetVideoMode(monitorHandle);

	u32 width = 0;
	u32 height = 0;
	u32 refresh = 0;

	if (mode) {

		width = mode->width;
		height = mode->height;
		refresh = mode->refreshRate;

	}

	return Monitor{id, static_cast<u32>(vx), static_cast<u32>(vy), static_cast<u32>(wx), static_cast<u32>(wy), static_cast<u32>(ww), static_cast<u32>(wh), static_cast<u32>(pw), static_cast<u32>(ph), width, height, refresh, name};

}



bool Window::monitorConfigurationChanged() {
	
	if (monitorsChanged) {

		monitorsChanged = false;
		return true;

	}

	return false;

}



void Window::setWindowMoveFunction(WindowMoveFunction function) {

	arc_assert(isOpen(), "Tried to set window move function for non-existing window");
	moveFunction = function;

	if (function) {

		glfwSetWindowPosCallback(windowHandle->handle, [](GLFWwindow* window, i32 x, i32 y) {
			Window* ptr = static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window))->window;
			ptr->moveFunction(x, y);
		});

	} else {
		glfwSetWindowPosCallback(windowHandle->handle, nullptr);
	}

}



void Window::setWindowResizeFunction(WindowResizeFunction function) {

	arc_assert(isOpen(), "Tried to set window resize function for non-existing window");
	resizeFunction = function;

	if (function) {

		glfwSetWindowSizeCallback(windowHandle->handle, [](GLFWwindow* window, i32 w, i32 h) {
			Window* ptr = static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window))->window;
			ptr->resizeFunction(w, h);
		});

	} else {
		glfwSetWindowSizeCallback(windowHandle->handle, nullptr);
	}

}



void Window::setWindowStateChangeFunction(WindowStateChangeFunction function) {

	arc_assert(isOpen(), "Tried to set window state change function for non-existing window");
	stateChangeFunction = function;

	if (function) {

		glfwSetWindowCloseCallback(windowHandle->handle, [](GLFWwindow* window) {
			Window* ptr = static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window))->window;
			ptr->stateChangeFunction(WindowState::CloseRequest);
		});

		glfwSetWindowFocusCallback(windowHandle->handle, [](GLFWwindow* window, i32 focused) {
			Window* ptr = static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window))->window;
			ptr->stateChangeFunction(focused ? WindowState::Focused : WindowState::Unfocused);
		});

		glfwSetWindowIconifyCallback(windowHandle->handle, [](GLFWwindow* window, i32 iconified) {
			Window* ptr = static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window))->window;
			ptr->stateChangeFunction(iconified ? WindowState::Minimized : WindowState::Restored);
		});

		glfwSetWindowMaximizeCallback(windowHandle->handle, [](GLFWwindow* window, i32 maximized) {
			Window* ptr = static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window))->window;
			ptr->stateChangeFunction(maximized ? WindowState::Maximized : WindowState::Restored);
		});


	} else {

		glfwSetWindowCloseCallback(windowHandle->handle, nullptr);
		glfwSetWindowFocusCallback(windowHandle->handle, nullptr);
		glfwSetWindowIconifyCallback(windowHandle->handle, nullptr);
		glfwSetWindowMaximizeCallback(windowHandle->handle, nullptr);

	}

}



void Window::setFramebufferResizeFunction(FramebufferResizeFunction function) {

	arc_assert(isOpen(), "Tried to set framebuffer resize function for non-existing window");
	fbResizeFunction = function;

	if (function) {

		glfwSetFramebufferSizeCallback(windowHandle->handle, [](GLFWwindow* window, i32 w, i32 h) {
			Window* ptr = static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window))->window;
			ptr->fbResizeFunction(w, h);
		});

	} else {
		glfwSetFramebufferSizeCallback(windowHandle->handle, nullptr);
	}

}



void Window::setDropFunction(DropFunction function) {

	arc_assert(isOpen(), "Tried to set drop function for non-existing window");
	dropFunction = function;

	if (function) {

		glfwSetDropCallback(windowHandle->handle, [](GLFWwindow* window, i32 count, const char** paths) {

			Window* ptr = static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window))->window;

			std::vector<std::string> pathStrings;
			pathStrings.reserve(count);

			for (i32 i = 0; i < count; i++) {
				pathStrings.emplace_back(*(paths + i));
			}

			ptr->dropFunction(std::move(pathStrings));

		});

	} else {
		glfwSetDropCallback(windowHandle->handle, nullptr);
	}

}



void Window::resetWindowFunctions() {

	setWindowMoveFunction(nullptr);
	setWindowResizeFunction(nullptr);
	setWindowStateChangeFunction(nullptr);
	setFramebufferResizeFunction(nullptr);
	setDropFunction(nullptr);

}



std::weak_ptr<WindowHandle> Window::getInternalHandle() const {
	return windowHandle;
}



bool Window::initialize() {

	static bool initialized = false;

	if (initialized) {
		return true;
	}

	if (!glfwInit()) {
		Log::error("Core", "Failed to initialize GLFW");
		return false;
	}

	initialized = true;
	return true;

}



void Window::shutdown() {
	glfwTerminate();
}



bool Window::setupGLContext() {
	return GLE::Core::init();
}



void Window::queryMonitors() {

	i32 count = 0;
	connectedMonitors = glfwGetMonitors(&count);
	monitorCount = count;

}



void Window::initMonitorCallback() {

	static bool init = false;

	if (!init) {

		init = true;
		glfwSetMonitorCallback([](GLFWmonitor* monitor, i32 state) {
			queryMonitors();
			monitorsChanged = true;
		});

		queryMonitors();

	}

}