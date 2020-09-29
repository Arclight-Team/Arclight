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



enum class WindowState {
	CloseRequest,
	Minimized,
	Restored,
	Focused,
	Unfocused,
	Maximized
};

struct GLFWwindow;
struct GLFWmonitor;

typedef void(*WindowMoveFunction)(Window*, u32, u32);
typedef void(*WindowResizeFunction)(Window*, u32, u32);
typedef void(*WindowStateChangeFunction)(Window*, WindowState);
typedef void(*FramebufferResizeFunction)(Window*, u32, u32);

class Window {

public:

	Window();

	void setWindowConfig(const WindowConfig& config);
	bool create(u32 w, u32 h, const std::string& title);
	bool createFullscreen(const std::string& title);
	void close();

	void setWindowed();
	void setFullscreen();

	void setSize(u32 w, u32 h);
	void setTitle(const std::string& title);
	void setX(u32 x);
	void setY(u32 y);
	void setPosition(u32 x, u32 y);
	void setLimits(u32 minW, u32 minH, u32 maxW, u32 maxH);
	void setMinLimits(u32 minW, u32 minH);
	void setMaxLimits(u32 maxW, u32 maxH);
	void setAspectRatio(double aspect);
	void setOpacity(double opacity);
	//void setIcon(); //Define when textures are implemented

	u32 getWidth() const;
	u32 getHeight() const;
	u32 getX() const;
	u32 getY() const;

	//void getIcon();

	void minimize();
	void restore();
	void maximize();
	void show();
	void hide();
	void focus();
	void requestAttention();
	void disableConstraints();

	void fetchEvents();
	void swapBuffers();

	bool isCreated() const;
	bool isFullscreen() const;

	void enableContext();
	void disableContext();
	void enableVSync();
	void disableVSync();

	void requestClose();
	void dismissCloseRequest();
	bool closeRequested() const;

	void setWindowMoveFunction(WindowMoveFunction function);
	void setWindowResizeFunction(WindowResizeFunction function);
	void setWindowStateChangeFunction(WindowStateChangeFunction function);
	void setFramebufferResizeFunction(FramebufferResizeFunction function);
	void resetWindowFunctions();

private:
	static bool setupGLContext();
	static bool queryMonitors();

	static GLFWmonitor** connectedMonitors;
	static GLFWmonitor* primaryMonitor;
	static u32 monitorCount;

	WindowConfig contextConfig;
	GLFWwindow* windowHandle;

	u32 backupWidth;
	u32 backupHeight;

	WindowMoveFunction moveFunction;
	WindowResizeFunction resizeFunction;
	WindowStateChangeFunction stateChangeFunction;
	FramebufferResizeFunction fbResizeFunction;

};