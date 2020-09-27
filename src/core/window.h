#pragma once

#include "types.h"
#include <string>


class Window;

class WindowConfig {

public:

	enum class OpenGLProfile {
		Core,
		Compatibility,
		Any
	};

	inline constexpr WindowConfig() : 
		openglMajor(3), openglMinor(3), profile(OpenGLProfile::Core), forwardContext(true), debugContext(false), srgbRendering(true),
		samples(1), redBits(8), greenBits(8), blueBits(8), alphaBits(8), depthBits(24), stencilBits(8),
		resizable(false), maximized(false), alwaysOnTop(false) {}

	WindowConfig& setOpenGLVersion(u32 major, u32 minor);
	WindowConfig& setOpenGLProfile(OpenGLProfile profile);
	WindowConfig& setOpenGLContextMode(bool forward, bool debug);
	WindowConfig& setSRGBMode(bool enable);
	WindowConfig& setSamples(u8 samples);
	WindowConfig& setFramebuffer(u8 redBits, u8 greenBits, u8 blueBits, u8 alphaBits, u8 depthBits, u8 stencilBits);
	WindowConfig& setResizable(bool enable);
	WindowConfig& setMaximized(bool enable);
	WindowConfig& setAlwaysOnTop(bool enable);

private:
	friend class Window;

	u32 openglMajor;
	u32 openglMinor;
	OpenGLProfile profile;
	bool forwardContext;
	bool debugContext;
	bool srgbRendering;
	u32 samples;
	u8 redBits;
	u8 greenBits;
	u8 blueBits;
	u8 alphaBits;
	u8 depthBits;
	u8 stencilBits;
	bool resizable;
	bool maximized;
	bool alwaysOnTop;

};


struct MonitorArea {
	u32 x;
	u32 y;
	u32 width;
	u32 height;
};

struct VideoMode {
	u32 width;
	u32 height;
	u32 redBits;
	u32 greenBits;
	u32 blueBits;
	u32 refreshRate;
};



struct GLFWwindow;
struct GLFWmonitor;


class Window {

public:

	Window();

	void setWindowConfig(const WindowConfig& config);
	bool create(u32 w, u32 h, const std::string& title);
	bool createFullscreen(const std::string& title);
	void close();


private:
	static void queryMonitors();

	static GLFWmonitor** connectedMonitors;
	static GLFWmonitor* primaryMonitor;
	static u32 monitorCount;

	WindowConfig contextConfig;
	GLFWwindow* windowHandle;

	u32 width;
	u32 height;
	bool created;
	bool fullscreen;

};