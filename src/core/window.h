#pragma once



class WindowConfig {

public:

	enum class OpenGLProfile {
		Core,
		Compatibility,
		Any
	};

	constexpr WindowConfig() {}

private:
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
	bool alwaysOnTop;
	bool maximized;

};


class GLFWwindow;

class Window {

public:

	constexpr Window() : windowHandle(nullptr) {}
	constexpr Window(u32 w, u32 h, const std::string& title) : windowHandle(nullptr) {}




private:
	GLFWwindow* windowHandle;

};