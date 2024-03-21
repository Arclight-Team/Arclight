/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Window.hpp
 */

#pragma once

#include "RenderCursor.hpp"
#include "WindowHandle.hpp"
#include "WindowConfig.hpp"
#include "Image/Pixel.hpp"
#include "Math/Vector.hpp"
#include "Common/Types.hpp"

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>



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

	using WindowRefreshFunction		= std::function<void()>;
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
	bool createNoContext(u32 w, u32 h, const std::string& title);
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

	void setCentered();

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
	i32 getX() const;
	i32 getY() const;
	Vec2i getPosition() const;
	Vec2ui getSize() const;
	Vec2ui getFramebufferSize() const;

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

	u32 getCurrentMonitorID() const;

	static u32 getMonitorCount();
	static Monitor getMonitor(u32 id);
	static bool monitorConfigurationChanged();

	void setWindowRefreshFunction(WindowRefreshFunction function);
	void setWindowMoveFunction(WindowMoveFunction function);
	void setWindowResizeFunction(WindowResizeFunction function);
	void setWindowStateChangeFunction(WindowStateChangeFunction function);
	void setFramebufferResizeFunction(FramebufferResizeFunction function);
	void setDropFunction(DropFunction function);
	void resetWindowFunctions();

	std::weak_ptr<WindowHandle> getInternalHandle() const;

private:

	void setupSyncCallback();
	void destroySyncCallback();

	static void initMonitorCallback();
	static void queryMonitors();
	static bool setupGLContext();

	static GLFWmonitor** connectedMonitors;
	static bool monitorsChanged;
	static u32 monitorCount;

	WindowConfig contextConfig;

	std::shared_ptr<WindowHandle> windowHandle;

	Vec2i backupPosition;
	Vec2ui backupSize;

	RenderCursor cursor;

	WindowRefreshFunction refreshFunction;
	WindowMoveFunction moveFunction;
	WindowResizeFunction resizeFunction;
	WindowStateChangeFunction stateChangeFunction;
	FramebufferResizeFunction fbResizeFunction;
	DropFunction dropFunction;

	inline static std::unordered_map<void*, std::function<void()>> syncCallbacks;

};