/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 window.hpp
 */

#pragma once

#include "rendercursor.hpp"
#include "windowhandle.hpp"
#include "windowconfig.hpp"
#include "image/pixel.hpp"
#include "math/vector.hpp"
#include "types.hpp"

#include <string>
#include <memory>
#include <functional>



template<Pixel P>
class Image;


struct Monitor {

	u32 monitorID;
	u32 virtualX;
	u32 virtualY;
	u32 workX;
	u32 workY;
	u32 workW;
	u32 workH;
	u32 physicalW;
	u32 physicalH;
	u32 width;
	u32 height;
	u32 refreshRate;
	std::string name;

};


enum class WindowState {
	CloseRequest,
	Minimized,
	Restored,
	Focused,
	Unfocused,
	Maximized
};

struct WindowHandle;
struct GLFWmonitor;


class Window final {

public:

	using WindowMoveFunction        = std::function<void(u32, u32)>;
	using WindowResizeFunction      = std::function<void(u32, u32)>;
	using WindowStateChangeFunction = std::function<void(WindowState)>;
	using FramebufferResizeFunction = std::function<void(u32, u32)>;
	using DropFunction              = std::function<void(const std::vector<std::string>&)>;

	Window();
	~Window();

	Window(const Window& window) = delete;
	Window& operator=(const Window& window) = delete;

	void setWindowConfig(const WindowConfig& config);
	bool create(u32 w, u32 h, const std::string& title);
	bool createFullscreen(const std::string& title, u32 monitorID = 0);
	void close();

	void setWindowed();
	void setFullscreen(u32 monitorID = 0);

	void setTitle(const std::string& title);

	void setX(u32 x);
	void setY(u32 y);
	void setPosition(const Vec2ui& pos);
	void setPosition(u32 x, u32 y);

	void setSize(const Vec2ui& size);
	void setSize(u32 w, u32 h);

	void setLimits(const Vec2ui& min, const Vec2ui& max);
	void setLimits(u32 minW, u32 minH, u32 maxW, u32 maxH);
	void setMinLimits(const Vec2ui& min);
	void setMinLimits(u32 minW, u32 minH);
	void setMaxLimits(const Vec2ui& max);
	void setMaxLimits(u32 maxW, u32 maxH);

	void setAspectRatio(double aspect);
	void setOpacity(double opacity);
	void setIcon(const Image<Pixel::RGBA8>& icon);
	void setAlwaysOnTop(bool onTop);

	u32 getWidth() const;
	u32 getHeight() const;
	u32 getFramebufferWidth() const;
	u32 getFramebufferHeight() const;
	u32 getX() const;
	u32 getY() const;
	Vec2ui getPosition() const;

	void minimize();
	void restore();
	void maximize();
	void show();
	void hide();
	void focus();
	void requestAttention();
	void disableConstraints();

	static void pollEvents();
	void swapBuffers();

	bool isOpen() const;
	bool isFullscreen() const;
	bool closeRequested() const;
	bool focused() const;
	bool minimized() const;
	bool maximized() const;
	bool hovered() const;
	bool visible() const;
	bool resizable() const;
	bool decorated() const;
	bool alwaysOnTop() const;
	
	void enableContext();
	void disableContext();
	void enableVSync();
	void disableVSync();

	void requestClose();
	void dismissCloseRequest();

	std::string getClipboardString() const;
	void setClipboardString(const std::string& str);
	
	RenderCursor& getRenderCursor();
	const RenderCursor& getRenderCursor() const;

	static u32 getMonitorCount();
	static Monitor getMonitor(u32 id);
	static bool monitorConfigurationChanged();

	void setWindowMoveFunction(WindowMoveFunction function);
	void setWindowResizeFunction(WindowResizeFunction function);
	void setWindowStateChangeFunction(WindowStateChangeFunction function);
	void setFramebufferResizeFunction(FramebufferResizeFunction function);
	void setDropFunction(DropFunction function);
	void resetWindowFunctions();

	std::weak_ptr<WindowHandle> getInternalHandle() const;

	static bool initialize();
	static void shutdown();

private:

	static void initMonitorCallback();
	static void queryMonitors();
	static bool setupGLContext();

	static GLFWmonitor** connectedMonitors;
	static bool monitorsChanged;
	static u32 monitorCount;

	WindowConfig contextConfig;

	std::shared_ptr<WindowHandle> windowHandle;

	u32 backupWidth;
	u32 backupHeight;

	RenderCursor cursor;

	WindowMoveFunction moveFunction;
	WindowResizeFunction resizeFunction;
	WindowStateChangeFunction stateChangeFunction;
	FramebufferResizeFunction fbResizeFunction;
	DropFunction dropFunction;

};