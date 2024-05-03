/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Window.cpp
 */

#include <vector>
#include <string>

#define Rectangle Rectangle_
#include "Common/Win32.hpp" // Force disable GDI's rectangle function
// Must be included before WindowHandle
#undef Rectangle

#include "Window/Window.hpp"
#include "Window/WindowHandle.hpp"
#include "Window/WindowClass.hpp"
#include "Window/WindowClassHandle.hpp"
#include "Monitor/Monitor.hpp"
#include "Image/Image.hpp"
#include "Common/Assert.hpp"
#include "Util/Log.hpp"
#include "OS/Common.hpp"

#include <dwmapi.h>


constexpr DWORD DwmDarkMode = 20;
constexpr DWORD DwmDarkModePre20H1 = 19;


Window::Window() : handle(nullptr), cursor(handle) {}

Window::~Window() {

    if (isOpen()) {
        close();
    }

}

bool Window::create(u32 viewportWidth, u32 viewportHeight, const std::string& title, WindowClass& wndClass) {

    if (isOpen()) {
        LogW("Window") << "Cannot open window that is already open";
        return true;
    }

    HMODULE hinst = GetModuleHandleW(nullptr);

    std::wstring wtitle = OS::String::toUTF16(title);

    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    DWORD exStyle = WS_EX_APPWINDOW;

    RECT rect = { 0, 0, static_cast<LONG>(viewportWidth), static_cast<LONG>(viewportHeight) };
    AdjustWindowRectEx(&rect, style, false, exStyle);

    HWND hwnd = CreateWindowExW(
        exStyle,
        wndClass.getInternalHandle().getWName().c_str(),
        wtitle.c_str(),
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        hinst,
        this
    );

    if (!hwnd) {
        LogE("Window") << "Failed to create window";
        return false;
    }

    handle = std::make_shared<WindowHandle>(hwnd, Vec2i(viewportWidth, viewportHeight));

    return true;
}

bool Window::createFullscreen(const std::string& title, WindowClass& wndClass) {

    Monitor monitor;
    if (!monitor.fromPrimary()) {
        return false;
    }

    return createFullscreen(monitor, title, wndClass);

}

bool Window::createFullscreen(Monitor& monitor, const std::string& title, WindowClass& wndClass) {

    Vec2ui monitorSize = monitor.getSize();

    if (!create(monitorSize.x, monitorSize.y, title, wndClass)) {
        return false;
    }

    setFullscreen(monitor);

    return true;

}

void Window::close() {

    if (!isOpen()) {
        LogW("Window") << "Cannot close window that is not open";
        return;
    }

    cursor.destroyAll();

    DestroyWindow(handle->hwnd);

    handle.reset();

}

void Window::setWindowed() {

    arc_assert(isOpen(), "Tried to set windowed mode for non-existing window");

    if (!handle->fullscreen.enabled) {
        return;
    }

    SetWindowLongW(handle->hwnd, GWL_STYLE, static_cast<LONG>(handle->fullscreen.backupStyle));
    SetWindowLongW(handle->hwnd, GWL_EXSTYLE, static_cast<LONG>(handle->fullscreen.backupExStyle));

    RECT& rect = handle->fullscreen.backupRect;
    SetWindowPos(handle->hwnd, nullptr,
                 rect.left, rect.top,
                 rect.right - rect.left, rect.bottom - rect.top,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    if (handle->maximized) {
        SendMessageW(handle->hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    }

    handle->fullscreen.enabled = false;

}

void Window::setFullscreen() {

    arc_assert(isOpen(), "Tried to set fullscreen mode for non-existing window");

    Monitor monitor;
    if (monitor.fromWindow(*this)) {
        setFullscreen(monitor);
    }

}

void Window::setFullscreen(Monitor& monitor) {

    arc_assert(isOpen(), "Tried to set fullscreen mode for non-existing window");

    if (handle->fullscreen.enabled) {
        return;
    }

    if (handle->maximized) {
        SendMessageW(handle->hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    }
    LONG style = GetWindowLongW(handle->hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLongW(handle->hwnd, GWL_EXSTYLE);

    handle->fullscreen.backupStyle = style;
    handle->fullscreen.backupExStyle = exStyle;
    GetWindowRect(handle->hwnd, &handle->fullscreen.backupRect);

    SetWindowLongW(handle->hwnd, GWL_STYLE, static_cast<LONG>(style & ~(WS_CAPTION | WS_THICKFRAME)));
    SetWindowLongW(handle->hwnd, GWL_EXSTYLE, static_cast<LONG>(exStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)));

    Vec2i monitorPos = monitor.getPosition();
    Vec2ui monitorSize = monitor.getSize();
    SetWindowPos(handle->hwnd, nullptr,
                 monitorPos.x, monitorPos.y,
                 static_cast<int>(monitorSize.x), static_cast<int>(monitorSize.y),
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    handle->fullscreen.enabled = true;

}

void Window::setTitle(const std::string& title) {

    arc_assert(isOpen(), "Tried to set window title for non-existing window");

    std::wstring wtitle = OS::String::toUTF16(title);
    SetWindowTextW(handle->hwnd, wtitle.c_str());

}

void Window::setX(i32 x) {

    arc_assert(isOpen(), "Tried to set window x-position for non-existing window");
    setPosition(x, getY());

}

void Window::setY(i32 y) {

    arc_assert(isOpen(), "Tried to set window y-position for non-existing window");
    setPosition(getX(), y);

}

void Window::setPosition(const Vec2i& pos) {
    setPosition(pos.x, pos.y);
}

void Window::setPosition(i32 x, i32 y) {

    arc_assert(isOpen(), "Tried to set window position for non-existing window");
    SetWindowPos(handle->hwnd, nullptr, static_cast<int>(x), static_cast<int>(y), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

}

void Window::setWidth(u32 width) {

    arc_assert(isOpen(), "Tried to set window width for non-existing window");
    setSize(width, getHeight());

}

void Window::setHeight(u32 height) {

    arc_assert(isOpen(), "Tried to set window height for non-existing window");
    setSize(getWidth(), height);

}

void Window::setSize(const Vec2ui& size) {
    setSize(size.x, size.y);
}

void Window::setSize(u32 w, u32 h) {

    arc_assert(isOpen(), "Tried to set window size for non-existing window");
    SetWindowPos(handle->hwnd, nullptr, 0, 0, static_cast<int>(w), static_cast<int>(h), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

}

void Window::setLimits(const Vec2ui& min, const Vec2ui& max) {
    setLimits(min.x, min.y, max.x, max.y);
}

void Window::setLimits(u32 minW, u32 minH, u32 maxW, u32 maxH) {

    arc_assert(isOpen(), "Tried to set window limits for non-existing window");
    handle->minSize = { minW, minH };
    handle->maxSize = { maxW, maxH };

}

void Window::setMinLimits(const Vec2ui& min) {
    setMinLimits(min.x, min.y);
}

void Window::setMinLimits(u32 minW, u32 minH) {

    arc_assert(isOpen(), "Tried to set window min limits for non-existing window");
    handle->minSize = { minW, minH };

}

void Window::setMaxLimits(const Vec2ui& max) {
    setMinLimits(max.x, max.y);
}

void Window::setMaxLimits(u32 maxW, u32 maxH) {

    arc_assert(isOpen(), "Tried to set window max limits for non-existing window");
    handle->maxSize = { maxW, maxH };

}

void Window::setAspectRatio(double aspect) {

    arc_assert(isOpen(), "Tried to set window aspect ratio for non-existing window");
    //glfwSetWindowAspectRatio(windowHandle->handle, aspect * 1000, 1000);

}

void Window::setOpacity(double opacity) {

    arc_assert(isOpen(), "Tried to set window opacity for non-existing window");
    //glfwSetWindowOpacity(windowHandle->handle, opacity);

}

void Window::setIcon(const Image<Pixel::RGBA8>& icon) {

    arc_assert(isOpen(), "Tried to set window icon for non-existing window");

    HICON hiconNew = WindowHandle::createIcon(icon, 0, 0, true);

    SendMessageW(handle->hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hiconNew));

    if (!handle->hiconSm) {
        SendMessageW(handle->hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hiconNew));
    }

    if (handle->hicon) {
        DestroyIcon(handle->hicon);
    }

    handle->hicon = hiconNew;
}

void Window::setSmallIcon(const Image<Pixel::RGBA8>& icon) {

    arc_assert(isOpen(), "Tried to set small window icon for non-existing window");

    HICON hiconNew = WindowHandle::createIcon(icon, 0, 0, true);

    SendMessageW(handle->hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hiconNew));

    if (handle->hiconSm) {
        DestroyIcon(handle->hiconSm);
    }

    handle->hiconSm = hiconNew;

}

void Window::setResizable(bool resizeable) {

    arc_assert(isOpen(), "Tried to set window resizeable on non-existing window");

    LONG style = GetWindowLongW(handle->hwnd, GWL_STYLE);

    if (resizeable) {
        style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
    } else {
        style &= ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
    }

    SetWindowLongW(handle->hwnd, GWL_STYLE, style);

    SetWindowPos(handle->hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

}

void Window::setDecorated(bool decorated) {

    arc_assert(isOpen(), "Tried to set window decorated on non-existing window");

    LONG style = GetWindowLongW(handle->hwnd, GWL_STYLE);

    if (decorated) {
        style = static_cast<LONG>((static_cast<ULONG>(style) & ~WS_POPUP) | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);
    } else {
        style = static_cast<LONG>((static_cast<ULONG>(style) & ~(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU)) | WS_POPUP);
    }

    SetWindowLongW(handle->hwnd, GWL_STYLE, style);

    SetWindowPos(handle->hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

}

void Window::setAlwaysOnTop(bool onTop) {

    arc_assert(isOpen(), "Tried to set window always-on-top on non-existing window");
    SetWindowPos(handle->hwnd, onTop ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

}

bool Window::setDarkMode(bool enabled) {

    arc_assert(isOpen(), "Tried to set window dark mode on non-existing window");
    BOOL darkMode = enabled;
    return DwmSetWindowAttribute(handle->hwnd, DwmDarkMode, &darkMode, sizeof(darkMode)) ||
           DwmSetWindowAttribute(handle->hwnd, DwmDarkModePre20H1, &darkMode, sizeof(darkMode));

}

void Window::resetIcon() {

    arc_assert(isOpen(), "Tried to reset window icon for non-existing window");

    HICON hiconNew = LoadIconW(nullptr, IDI_APPLICATION);

    SendMessageW(handle->hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hiconNew));

    if (!handle->hiconSm) {
        SendMessageW(handle->hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hiconNew));
    }

    if (handle->hicon) {
        DestroyIcon(handle->hicon);

        handle->hicon = nullptr;
    }

}

void Window::resetSmallIcon() {

    arc_assert(isOpen(), "Tried to reset small window icon for non-existing window");

    HICON hiconNew = handle->hicon ? handle->hicon : LoadIconW(nullptr, IDI_APPLICATION);

    SendMessageW(handle->hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hiconNew));

    if (handle->hiconSm) {
        DestroyIcon(handle->hiconSm);

        handle->hiconSm = nullptr;
    }

}

i32 Window::getX() const {

    arc_assert(isOpen(), "Tried to get window x-position for non-existing window");
    return getPosition().x;

}

i32 Window::getY() const {

    arc_assert(isOpen(), "Tried to get window y-position for non-existing window");
    return getPosition().y;

}

Vec2i Window::getPosition() const {

    arc_assert(isOpen(), "Tried to get window position for non-existing window");

    RECT rect;
    GetWindowRect(handle->hwnd, &rect);

    return { rect.left, rect.top };

}

u32 Window::getWidth() const {

    arc_assert(isOpen(), "Tried to get window width for non-existing window");
    return getSize().x;

}

u32 Window::getHeight() const {

    arc_assert(isOpen(), "Tried to get window height for non-existing window");
    return getSize().y;

}

Vec2ui Window::getSize() const {

    arc_assert(isOpen(), "Tried to get window size for non-existing window");

    RECT rect;
    GetWindowRect(handle->hwnd, &rect);

    return { rect.right - rect.left, rect.bottom - rect.top };

}

u32 Window::getViewportWidth() const {

    arc_assert(isOpen(), "Tried to get window viewport width for non-existing window");
    return handle->viewportSize.x;

}

u32 Window::getViewportHeight() const {

    arc_assert(isOpen(), "Tried to get window viewport height for non-existing window");
    return handle->viewportSize.y;

}

Vec2ui Window::getViewportSize() const {

    arc_assert(isOpen(), "Tried to get viewport size for non-existing window");
    return handle->viewportSize;

}

void Window::minimize() {

    arc_assert(isOpen(), "Tried to minimize window for non-existing window");
    ShowWindow(handle->hwnd, SW_MINIMIZE);

}

void Window::restore() {

    arc_assert(isOpen(), "Tried to restore window for non-existing window");
    ShowWindow(handle->hwnd, SW_RESTORE);

}

void Window::maximize() {

    arc_assert(isOpen(), "Tried to maximize window for non-existing window");

    if (handle->fullscreen.enabled) {
        handle->maximized = true; // set maximized for when exiting fullscreen
        return;
    }

    ShowWindow(handle->hwnd, SW_MAXIMIZE);
    // TODO: maximize when window is not visible
    // CHECK https://github.com/glfw/glfw/blob/bf945f1213728a98f7647380616f9cff9f6b3611/src/win32_window.c#L1794

}

void Window::show() {

    arc_assert(isOpen(), "Tried to show window for non-existing window");
    ShowWindow(handle->hwnd, SW_SHOW);

}

void Window::hide() {

    arc_assert(isOpen(), "Tried to hide window for non-existing window");
    ShowWindow(handle->hwnd, SW_HIDE);

}

void Window::focus() {

    arc_assert(isOpen(), "Tried to focus window for non-existing window");
    BringWindowToTop(handle->hwnd);
    SetForegroundWindow(handle->hwnd);
    SetFocus(handle->hwnd);

}

void Window::requestAttention() {

    arc_assert(isOpen(), "Tried to request attention for non-existing window");
    FlashWindow(handle->hwnd, TRUE);

}

void Window::disableConstraints() {

    arc_assert(isOpen(), "Tried to disable size constraints for non-existing window");
    handle->minSize = { -1, -1 };
    handle->maxSize = { -1, -1 };

    //glfwSetWindowAspectRatio(windowHandle->handle, GLFW_DONT_CARE, GLFW_DONT_CARE);

}

void Window::center() {

    arc_assert(isOpen(), "Tried to center non-existing window");

    Monitor monitor;
    if (monitor.fromWindow(*this)) {
        center(monitor);
    }

}

void Window::center(Monitor& monitor) {

    arc_assert(isOpen(), "Tried to center non-existing window");

    Vec2i monitorPos = monitor.getPosition();
    Vec2ui monitorSize = monitor.getSize();
    Vec2ui windowSize = getSize();

    setPosition(
        monitorPos.x + ((static_cast<i32>(monitorSize.x) - static_cast<i32>(windowSize.x)) / 2),
        monitorPos.y + ((static_cast<i32>(monitorSize.y) - static_cast<i32>(windowSize.y)) / 2)
    );

}

void Window::pollEvents() {

    arc_assert(isOpen(), "Tried to poll events of non-existing window");

    MSG msg;
    while (PeekMessageW(&msg, handle->hwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

}

void Window::requestClose() {

    arc_assert(isOpen(), "Tried to request close for non-existing window");
    handle->closeRequested = true;

}

void Window::dismissCloseRequest() {

    arc_assert(isOpen(), "Tried to dismiss close request for non-existing window");
    handle->closeRequested = false;

}

std::string Window::getClipboardString() const {

    arc_assert(isOpen(), "Tried to obtain clipboard string for non-existing window");

    /*const char* str = glfwGetClipboardString(windowHandle->handle);

    if (str) {
        return str;
    } else {
        return "";
    }*/

    return "";

}

void Window::setClipboardString(const std::string& str) {

    arc_assert(isOpen(), "Tried to set clipboard string for non-existing window");
    //glfwSetClipboardString(windowHandle->handle, str.c_str());

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
    return handle != nullptr;
}

bool Window::isFullscreen() const {

    arc_assert(isOpen(), "Tried to obtain fullscreen state for non-existing window");
    return handle->fullscreen.enabled;

}

bool Window::closeRequested() const {

    arc_assert(isOpen(), "Tried to fetch close request state for non-existing window");
    return handle->closeRequested;

}

bool Window::focused() const {

    arc_assert(isOpen(), "Tried to fetch focus state for non-existing window");
    return handle->focused;

}

bool Window::minimized() const {

    arc_assert(isOpen(), "Tried to fetch minimized state for non-existing window");
    return handle->minimized;

}

bool Window::maximized() const {

    arc_assert(isOpen(), "Tried to fetch maximized state for non-existing window");
    return handle->maximized;

}

bool Window::hovered() const {

    arc_assert(isOpen(), "Tried to fetch hovered state for non-existing window");
    return handle->hovered;

}

bool Window::visible() const {

    arc_assert(isOpen(), "Tried to fetch visibility state for non-existing window");
    return IsWindowVisible(handle->hwnd);

}

bool Window::resizable() const {

    arc_assert(isOpen(), "Tried to fetch resizeable state for non-existing window");
    return GetWindowLongW(handle->hwnd, GWL_STYLE) & WS_THICKFRAME;

}

bool Window::decorated() const {

    arc_assert(isOpen(), "Tried to fetch decoration state for non-existing window");
    return GetWindowLongW(handle->hwnd, GWL_STYLE) & WS_CAPTION;

}

bool Window::alwaysOnTop() const {

    arc_assert(isOpen(), "Tried to fetch always-on-top state for non-existing window");
    return GetWindowLongW(handle->hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;

}

void Window::setRefreshFunction(const RefreshFunction& function) {

    arc_assert(isOpen(), "Tried to set window refresh function for non-existing window");
    handle->refreshFunction = function;

}

void Window::setMoveFunction(const MoveFunction& function) {

    arc_assert(isOpen(), "Tried to set window move function for non-existing window");
    handle->moveFunction = function;

}

void Window::setResizeFunction(const ResizeFunction& function) {

    arc_assert(isOpen(), "Tried to set window resize function for non-existing window");
    handle->resizeFunction = function;

}

void Window::setStateChangeFunction(const StateChangeFunction& function) {

    arc_assert(isOpen(), "Tried to set window state change function for non-existing window");
    handle->stateChangeFunction = function;

}

void Window::setDropFunction(const DropFunction& function) {

    arc_assert(isOpen(), "Tried to set drop function for non-existing window");
    handle->dropFunction = function;

}

void Window::resetWindowFunctions() {

    setRefreshFunction(nullptr);
    setMoveFunction(nullptr);
    setResizeFunction(nullptr);
    setStateChangeFunction(nullptr);
    setDropFunction(nullptr);

}

std::weak_ptr<WindowHandle> Window::getInternalHandle() const {

    arc_assert(isOpen(), "Tried to get internal handle for non-existing window");
    return handle;

}



WindowHandle::WindowHandle(HWND hwnd, const Vec2i& viewport) {

	this->hwnd = hwnd;
	viewportSize = viewport;
	closeRequested = false;
	fullscreen.enabled = false;
	fullscreen.backupRect = { 0, 0, 0, 0 };
	fullscreen.backupStyle = 0;
	fullscreen.backupExStyle = 0;
	minSize = { -1, -1 };
	maxSize = { -1, -1 };
	resizing = false;
	focused = false;
	minimized = false;
	maximized = false;
	hovered = false;
	resizeable = false;
	decorated = false;
	hicon = nullptr;
	hiconSm = nullptr;
	refreshFunction = nullptr;
	moveFunction = nullptr;
	resizeFunction = nullptr;
	stateChangeFunction = nullptr;
	dropFunction = nullptr;
	messageHandlerFunction = defaultMessageHandler;

}

LRESULT CALLBACK WindowHandle::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept {

    if (uMsg == WM_NCCREATE) {

        auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        auto* w = static_cast<Window*>(cs->lpCreateParams);

        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(w));

    } else if (auto* w = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))) {

        if (w->isOpen() && w->handle->messageHandlerFunction) {
            return w->handle->messageHandlerFunction(*w, hwnd, uMsg, wParam, lParam);
        }

    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);

}

LRESULT WindowHandle::defaultMessageHandler(Window& w, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept {

    switch (uMsg) {

        case WM_MENUCHAR: {
            return MAKELRESULT(0, MNC_CLOSE);
        }

        case WM_SYSCOMMAND: {

            if ((wParam & 0xFFF0) == SC_SIZE) {
                w.handle->resizing = true;
                break;
            }
            break;

        }

        case WM_ENTERSIZEMOVE: {

            if (w.handle->resizing) {
                notifyStateChange(w, WindowState::BeginResize);
                return 0;
            }
            break;

        }

        case WM_EXITSIZEMOVE: {

            if (w.handle->resizing) {
                w.handle->resizing = false;
                notifyStateChange(w, WindowState::EndResize);
                return 0;
            }
            break;

        }

        case WM_SIZE: {

            Vec2i viewportSize = Vec2i(LOWORD(lParam), HIWORD(lParam));

            if (w.handle->viewportSize.x != viewportSize.x ||
                w.handle->viewportSize.y != viewportSize.y) {

                w.handle->viewportSize = viewportSize;

                if (w.handle->resizeFunction) {
                    w.handle->resizeFunction(w, viewportSize.x, viewportSize.y);
                }

            }

            bool minimized = wParam == SIZE_MINIMIZED;
            bool maximized = wParam == SIZE_MAXIMIZED || (w.handle->maximized && wParam != SIZE_RESTORED);

            if (w.handle->minimized != minimized) {
                notifyStateChange(w, minimized ? WindowState::Minimized : WindowState::Restored);
                w.handle->minimized = minimized;
            }

            if (w.handle->maximized != maximized) {
                notifyStateChange(w, maximized ? WindowState::Maximized : WindowState::Restored);
                w.handle->maximized = maximized;
            }

            return 0;

        }

        case WM_MOVE: {

            // i16 cast required because LOWORD/HIWORD returns an
            // unsigned value and windows can move to negative positions
            i32 x = static_cast<i16>(LOWORD(lParam));
            i32 y = static_cast<i16>(HIWORD(lParam));

            if (w.handle->moveFunction) {
                w.handle->moveFunction(w, x, y);
            }

            return 0;

        }

        case WM_GETMINMAXINFO: {

            auto* mmi = reinterpret_cast<MINMAXINFO*>(lParam);

            if (w.handle->minSize != Vec2ui(-1, -1)) {

                mmi->ptMinTrackSize.x = static_cast<LONG>(w.handle->minSize.x);
                mmi->ptMinTrackSize.y = static_cast<LONG>(w.handle->minSize.y);

            }

            if (w.handle->maxSize != Vec2ui(-1, -1)) {

                mmi->ptMaxTrackSize.x = static_cast<LONG>(w.handle->maxSize.x);
                mmi->ptMaxTrackSize.y = static_cast<LONG>(w.handle->maxSize.y);

            }

            return 0;

        }

        case WM_DROPFILES: {

            if (!w.handle->dropFunction) {
                break;
            }

            auto hdrop = reinterpret_cast<HDROP>(wParam);

            SizeT count = DragQueryFileW(hdrop, 0xFFFFFFFF, nullptr, 0);
            std::vector<std::string> paths;

            // Notify that the mouse cursor has moved, otherwise
            // we don't know where to drop the file in our window
            POINT cursorPt;
            DragQueryPoint(hdrop, &cursorPt);
            // TODO
            //_glfwInputCursorPos(window, pt.x, pt.y);

            for (SizeT i = 0; i < count; i++) {
                SizeT length = DragQueryFileW(hdrop, i, nullptr, 0);
                auto buffer = std::make_unique<WCHAR[]>(length + 1);

                DragQueryFileW(hdrop, i, buffer.get(), length + 1);
                paths[i] = OS::String::toUTF8(buffer.get());
            }

            w.handle->dropFunction(w, paths);

            DragFinish(hdrop);
            return 0;

        }

        case WM_SETFOCUS: {

            w.handle->focused = true;
            notifyStateChange(w, WindowState::Focused);
            return 0;

        }

        case WM_KILLFOCUS: {

            w.handle->focused = false;
            notifyStateChange(w, WindowState::Unfocused);
            return 0;

        }

        case WM_CLOSE: {

            w.handle->closeRequested = true;
            notifyStateChange(w, WindowState::CloseRequest);
            return 0;
        }

        case WM_DESTROY: {
            // DO NOTHING FOR NOW
            return 0;
        }

        default: break;

    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);

}

void WindowHandle::notifyStateChange(Window& w, WindowState state) noexcept {

    if (w.handle->stateChangeFunction) {
        w.handle->stateChangeFunction(w, state);
    }

}

HICON WindowHandle::createIcon(const Image<Pixel::RGBA8>& image, int xhot, int yhot, bool icon) {

    u32 width = image.getWidth();
    u32 height = image.getHeight();

    BITMAPV5HEADER bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.bV5Size        = sizeof(bi);
    bi.bV5Width       = static_cast<i32>(width);
    bi.bV5Height      = -static_cast<i32>(height);
    bi.bV5Planes      = 1;
    bi.bV5BitCount    = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask     = 0x00FF0000;
    bi.bV5GreenMask   = 0x0000FF00;
    bi.bV5BlueMask    = 0x000000FF;
    bi.bV5AlphaMask   = 0xFF000000;

    HDC hdc = GetDC(nullptr);
    u8* target = nullptr;
    HBITMAP color = CreateDIBSection(hdc, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, reinterpret_cast<void**>(&target), nullptr, 0);
    ReleaseDC(nullptr, hdc);

    if (!color) {
        LogE("Window") << "Failed to create RGBA bitmap";
        return nullptr;
    }

    HBITMAP mask = CreateBitmap(static_cast<i32>(width), static_cast<i32>(height), 1, 1, nullptr);

    if (!mask) {
        LogE("Window") << "Failed to create mask bitmap";
        DeleteObject(color);
        return nullptr;
    }

    const u8* source = image.getImageData();

    for (SizeT i = 0; i < width * height; i++) {
        target[0] = source[2];
        target[1] = source[1];
        target[2] = source[0];
        target[3] = source[3];
        target += 4;
        source += 4;
    }

    ICONINFO iconInfo;
    ZeroMemory(&iconInfo, sizeof(iconInfo));
    iconInfo.fIcon    = icon;
    iconInfo.xHotspot = xhot;
    iconInfo.yHotspot = yhot;
    iconInfo.hbmMask  = mask;
    iconInfo.hbmColor = color;

    HICON hicon = CreateIconIndirect(&iconInfo);

    DeleteObject(color);
    DeleteObject(mask);

    if (!hicon) {

        if (icon) {
            LogE("Window") << "Failed to create icon";
        } else {
            LogE("Window") << "Failed to create cursor";
        }

    }

    return hicon;

}

void WindowHandle::setMessageHandlerFunction(const WindowHandle::MessageHandlerFunction& function) {

    messageHandlerFunction = function;

}
