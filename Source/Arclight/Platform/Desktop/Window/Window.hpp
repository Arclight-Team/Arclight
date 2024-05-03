/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Window.hpp
 */

#pragma once

#include "RenderCursor.hpp"
#include "Image/Pixel.hpp"
#include "Math/Vector.hpp"
#include "Common/Types.hpp"
#include "StdExt/OptionalRef.hpp"

#include <string>
#include <memory>
#include <functional>



template<Pixel P>
class Image;


enum class WindowState {
    CloseRequest,
    Minimized,
    Restored,
    Focused,
    Unfocused,
    Maximized,
    BeginResize,
    EndResize
};

class Monitor;

class WindowHandle;
class WindowClass;

class Window final {

public:
    using RefreshFunction     = std::function<void(Window& window)>;
    using MoveFunction        = std::function<void(Window& window, u32 x, u32 y)>;
    using ResizeFunction      = std::function<void(Window& window, u32 viewportWidth, u32 viewportHeight)>;
    using StateChangeFunction = std::function<void(Window& window, WindowState state)>;
    using DropFunction        = std::function<void(Window& window, const std::vector<std::string>& paths)>;

    Window();
    ~Window();

    Window(const Window& window) = delete;
    Window& operator=(const Window& window) = delete;

    bool create(u32 viewportWidth, u32 viewportHeight, const std::string& title, WindowClass& wndClass);
    bool createFullscreen(const std::string& title, WindowClass& wndClass);
    bool createFullscreen(Monitor& monitor, const std::string& title, WindowClass& wndClass);
    void close();

    void setWindowed();
    void setFullscreen();
    void setFullscreen(Monitor& monitor);

    void setTitle(const std::string& title);

    void setX(i32 x);
    void setY(i32 y);
    void setPosition(const Vec2i& pos);
    void setPosition(i32 x, i32 y);

    void setWidth(u32 width);
    void setHeight(u32 height);
    void setSize(const Vec2ui& size);
    void setSize(u32 width, u32 height);

    void setLimits(const Vec2ui& min, const Vec2ui& max);
    void setLimits(u32 minW, u32 minH, u32 maxW, u32 maxH);
    void setMinLimits(const Vec2ui& min);
    void setMinLimits(u32 minW, u32 minH);
    void setMaxLimits(const Vec2ui& max);
    void setMaxLimits(u32 maxW, u32 maxH);

    void setAspectRatio(double aspect);
    void setOpacity(double opacity);
    void setIcon(const Image<Pixel::RGBA8>& icon);
    void setSmallIcon(const Image<Pixel::RGBA8>& smallIcon);
    void setResizable(bool resizeable);
    void setDecorated(bool decorated);
    void setAlwaysOnTop(bool onTop);
    bool setDarkMode(bool enabled);

    void resetIcon();
    void resetSmallIcon();

    i32 getX() const;
    i32 getY() const;
    Vec2i getPosition() const;
    u32 getWidth() const;
    u32 getHeight() const;
    Vec2ui getSize() const;
    u32 getViewportWidth() const;
    u32 getViewportHeight() const;
    Vec2ui getViewportSize() const;

    void minimize();
    void restore();
    void maximize();
    void show();
    void hide();
    void focus();
    void requestAttention();
    void disableConstraints();
    void center();
    void center(Monitor& monitor);

    void pollEvents();

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

    void requestClose();
    void dismissCloseRequest();

    std::string getClipboardString() const;
    void setClipboardString(const std::string& str);

    RenderCursor& getRenderCursor();
    const RenderCursor& getRenderCursor() const;

    void setRefreshFunction(const RefreshFunction& function);
    void setMoveFunction(const MoveFunction& function);
    void setResizeFunction(const ResizeFunction& function);
    void setStateChangeFunction(const StateChangeFunction& function);
    void setDropFunction(const DropFunction& function);
    void resetWindowFunctions();

    std::weak_ptr<WindowHandle> getInternalHandle() const;

private:

    std::shared_ptr<WindowHandle> handle;

    RenderCursor cursor;

    friend WindowHandle;
};
