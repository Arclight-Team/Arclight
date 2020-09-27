#include "core/window.h"
#include "util/assert.h"

#include <GLFW/glfw3.h>




WindowConfig& WindowConfig::setOpenGLVersion(u32 major, u32 minor) {

	arc_assert((major == 3 || major == 4) && minor <= 6, "OpenGL version %d.%d not supported", major, minor);
	this->openglMajor = major;
	this->openglMinor = minor;
	return *this;

}



WindowConfig& WindowConfig::setOpenGLProfile(OpenGLProfile profile) {

	this->profile = profile;
	return *this;

}



WindowConfig& WindowConfig::setOpenGLContextMode(bool forward, bool debug) {

	this->forwardContext = forward;
	this->debugContext = debug;
	return *this;

}



WindowConfig& WindowConfig::setSRGBMode(bool enable) {

	this->srgbRendering = enable;
	return *this;

}



WindowConfig& WindowConfig::setSamples(u8 samples) {

	this->samples = samples;
	return *this;

}



WindowConfig& WindowConfig::setFramebuffer(u8 redBits, u8 greenBits, u8 blueBits, u8 alphaBits, u8 depthBits, u8 stencilBits) {

	this->redBits = redBits;
	this->greenBits = greenBits;
	this->blueBits = blueBits;
	this->alphaBits = alphaBits;
	this->depthBits = depthBits;
	this->stencilBits = stencilBits;
	return *this;

}



WindowConfig& WindowConfig::setResizable(bool enable) {

	this->resizable = enable;
	return *this;

}



WindowConfig& WindowConfig::setMaximized(bool enable) {

	this->maximized = enable;
	return *this;

}



WindowConfig& WindowConfig::setAlwaysOnTop(bool enable) {

	this->alwaysOnTop = enable;
	return *this;

}



GLFWmonitor** Window::connectedMonitors = nullptr;
GLFWmonitor* Window::primaryMonitor = nullptr;
u32 Window::monitorCount = 0;



Window::Window() : backupWidth(0), backupHeight(0), windowHandle(nullptr) {}



void Window::setWindowConfig(const WindowConfig& config) {

	if (isCreated()) {
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

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, config.debugContext);
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

	if (isCreated()) {
		Log::warn("Window", "Cannot open window that is already open");
		return true;
	}

	windowHandle = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);

	if (windowHandle) {
		enableContext();
	} else {
		Log::error("Window", "Failed to create window");
	}

	return isCreated();

}



bool Window::createFullscreen(const std::string& title) {

	if (isCreated()) {
		Log::warn("Window", "Cannot open window that is already open");
		return true;
	}

	if (!queryMonitors()) {
		Log::error("Window", "No valid monitor found");
		return false;
	}

	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

	contextConfig.redBits = videoMode->redBits;
	contextConfig.greenBits = videoMode->greenBits;
	contextConfig.blueBits = videoMode->blueBits;

	glfwWindowHint(GLFW_RED_BITS, contextConfig.redBits);
	glfwWindowHint(GLFW_GREEN_BITS, contextConfig.greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, contextConfig.blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);

	windowHandle = glfwCreateWindow(videoMode->width, videoMode->height, title.c_str(), primaryMonitor, nullptr);

	if (windowHandle) {

		backupWidth = videoMode->width;
		backupHeight = videoMode->height;
		enableContext();

	} else {
		Log::error("Window", "Failed to create window");
	}

	return isCreated();

}



void Window::close() {

	if (!isCreated()) {
		Log::warn("Window", "Cannot close window that is not open");
		return;
	}

	glfwDestroyWindow(windowHandle);
	
	backupWidth = 0;
	backupHeight = 0;
	windowHandle = nullptr;

}



void Window::setWindowedMode() {

	arc_assert(isCreated(), "Tried to set windowed mode for non-existing window");

	if (!isFullscreen()) {
		return;
	}

	glfwSetWindowMonitor(windowHandle, nullptr, 0, 0, backupWidth, backupHeight, GLFW_DONT_CARE);

}



void Window::setFullscreen() {

	arc_assert(isCreated(), "Tried to set fullscreen mode for non-existing window");

	if (isFullscreen()) {
		return;
	}

	if (!queryMonitors()) {
		Log::error("Window", "No valid monitor found");
		return;
	}

	backupWidth = getWidth();
	backupHeight = getHeight();

	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
	glfwSetWindowMonitor(windowHandle, primaryMonitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);

}



void Window::setSize(u32 w, u32 h) {

	arc_assert(isCreated(), "Tried to set window size for non-existing window");
	glfwSetWindowSize(windowHandle, w, h);

}



void Window::setTitle(const std::string& title) {

	arc_assert(isCreated(), "Tried to set window title for non-existing window");
	glfwSetWindowTitle(windowHandle, title.c_str());

}



void Window::setX(u32 x) {
		
	arc_assert(isCreated(), "Tried to set window x-position for non-existing window");
	glfwSetWindowPos(windowHandle, x, getY());

}



void Window::setY(u32 y) {

	arc_assert(isCreated(), "Tried to set window y-position for non-existing window");
	glfwSetWindowPos(windowHandle, getX(), y);

}



void Window::setPosition(u32 x, u32 y) {

	arc_assert(isCreated(), "Tried to set window position for non-existing window");
	glfwSetWindowPos(windowHandle, x, y);

}



void Window::setLimits(u32 minW, u32 minH, u32 maxW, u32 maxH) {

	arc_assert(isCreated(), "Tried to set window limits for non-existing window");
	glfwSetWindowSizeLimits(windowHandle, minW, minH, maxW, maxH);

}



void Window::setMinLimits(u32 minW, u32 minH) {

	arc_assert(isCreated(), "Tried to set window min limits for non-existing window");
	glfwSetWindowSizeLimits(windowHandle, minW, minH, GLFW_DONT_CARE, GLFW_DONT_CARE);

}



void Window::setMaxLimits(u32 maxW, u32 maxH) {

	arc_assert(isCreated(), "Tried to set window max limits for non-existing window");
	glfwSetWindowSizeLimits(windowHandle, GLFW_DONT_CARE, GLFW_DONT_CARE, maxW, maxH);

}



void Window::setAspectRatio(double aspect) {

	arc_assert(isCreated(), "Tried to set window aspect ratio for non-existing window");
	glfwSetWindowAspectRatio(windowHandle, aspect * 1000, 1000);

}



void Window::setOpacity(double opacity) {

	arc_assert(isCreated(), "Tried to set window opacity for non-existing window");
	glfwSetWindowOpacity(windowHandle, opacity);

}



u32 Window::getWidth() const {

	arc_assert(isCreated(), "Tried to get window width for non-existing window");

	i32 w = 0;
	glfwGetWindowSize(windowHandle, &w, nullptr);
	return w;
		
}



u32 Window::getHeight() const {

	arc_assert(isCreated(), "Tried to get window height for non-existing window");

	i32 h = 0;
	glfwGetWindowSize(windowHandle, nullptr, &h);
	return h;

}



u32 Window::getX() const {

	arc_assert(isCreated(), "Tried to get window x-position for non-existing window");

	i32 x = 0;
	glfwGetWindowPos(windowHandle, &x, nullptr);
	return x;

}



u32 Window::getY() const {

	arc_assert(isCreated(), "Tried to get window y-position for non-existing window");

	i32 y = 0;
	glfwGetWindowPos(windowHandle, nullptr, &y);
	return y;

}



void Window::minimize() {

	arc_assert(isCreated(), "Tried to minimize window for non-existing window");
	glfwIconifyWindow(windowHandle);

}



void Window::restore() {

	arc_assert(isCreated(), "Tried to restore window for non-existing window");
	glfwRestoreWindow(windowHandle);

}



void Window::maximize() {

	arc_assert(isCreated(), "Tried to maximize window for non-existing window");
	glfwMaximizeWindow(windowHandle);

}



void Window::show() {

	arc_assert(isCreated(), "Tried to show window for non-existing window");
	glfwShowWindow(windowHandle);

}



void Window::hide() {

	arc_assert(isCreated(), "Tried to hide window for non-existing window");
	glfwHideWindow(windowHandle);

}



void Window::focus() {

	arc_assert(isCreated(), "Tried to focus window for non-existing window");
	glfwFocusWindow(windowHandle);

}



void Window::requestAttention() {

	arc_assert(isCreated(), "Tried to request attention for non-existing window");
	glfwRequestWindowAttention(windowHandle);

}



void Window::disableConstraints() {

	arc_assert(isCreated(), "Tried to disable size constraints for non-existing window");
	glfwSetWindowSizeLimits(windowHandle, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetWindowAspectRatio(windowHandle, GLFW_DONT_CARE, GLFW_DONT_CARE);

}



void Window::fetchEvents() {
	glfwPollEvents();
}



void Window::swapBuffers() {

	arc_assert(isCreated(), "Tried to swap buffers for non-existing window");
	glfwSwapBuffers(windowHandle);

}



void Window::enableContext() {

	arc_assert(isCreated(), "Tried to enable OpenGL context for non-existing window");
	glfwMakeContextCurrent(windowHandle);

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



bool Window::isCreated() const {
	return windowHandle != nullptr;
}



bool Window::isFullscreen() const {

	arc_assert(isCreated(), "Tried to obtain fullscreen state for non-existing window");
	return glfwGetWindowMonitor(windowHandle) != nullptr;

}



bool Window::queryMonitors() {

	i32 count = 0;
	connectedMonitors = glfwGetMonitors(&count);
	primaryMonitor = glfwGetPrimaryMonitor();
	monitorCount = count;

	return primaryMonitor != nullptr;

}