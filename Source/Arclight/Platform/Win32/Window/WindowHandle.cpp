/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 WindowHandle.cpp
 */

#include "WindowHandle.hpp"
#include "OS/Common.hpp"
#include "Image/Image.hpp"



WindowHandle::WindowHandle(HWND hwnd, const Vec2ui& viewportSize) {

	this->hwnd = hwnd;
	this->viewportSize = viewportSize;
	closeRequested = false;
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
	windowMessageHandlerFunction = defaultWindowMessageHandler;
	inputMessageHandlerFunction = nullptr;

}

LRESULT CALLBACK WindowHandle::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (uMsg == WM_NCCREATE) {

		auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		auto* w = static_cast<Window*>(cs->lpCreateParams);

		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(w));

	} else if (auto* window = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA))) {

		if (window->isOpen()) {

			auto handle = window->getInternalHandle().lock();

			if (handle) {

				std::optional<LRESULT> windowResult;
				std::optional<LRESULT> inputResult;

				if (handle->windowMessageHandlerFunction) {
					windowResult = handle->windowMessageHandlerFunction(*window, hwnd, uMsg, wParam, lParam);
				}

				if (handle->inputMessageHandlerFunction) {
					inputResult = handle->inputMessageHandlerFunction(hwnd, uMsg, wParam, lParam);
				}

				if (windowResult.has_value()) {
					return *windowResult;
				} else if (inputResult.has_value()) {
					return *inputResult;
				}

			}

		}

	}

	return DefWindowProcW(hwnd, uMsg, wParam, lParam);

}

std::optional<LRESULT> WindowHandle::defaultWindowMessageHandler(Window& window, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	auto handle = window.getInternalHandle().lock();
	auto notifyStateChange = [&window, &handle] (WindowState state) { if (handle->stateChangeFunction) { handle->stateChangeFunction(window, state); }};

	switch (uMsg) {

		case WM_MENUCHAR: {
			return MAKELRESULT(0, MNC_CLOSE);
		}

		case WM_SYSCOMMAND: {

			if ((wParam & 0xFFF0) == SC_SIZE) {
				handle->resizing = true;
				break;
			}
			break;

		}

		case WM_ENTERSIZEMOVE: {

			if (handle->resizing) {
				notifyStateChange(WindowState::BeginResize);
				return 0;
			}
			break;

		}

		case WM_EXITSIZEMOVE: {

			if (handle->resizing) {
				handle->resizing = false;
				notifyStateChange(WindowState::EndResize);
				return 0;
			}
			break;

		}

		case WM_SIZE: {

			Vec2ui viewportSize = Vec2ui(LOWORD(lParam), HIWORD(lParam));

			if (handle->viewportSize.x != viewportSize.x ||
				handle->viewportSize.y != viewportSize.y) {

				handle->viewportSize = viewportSize;

				if (handle->resizeFunction) {
					handle->resizeFunction(window, viewportSize.x, viewportSize.y);
				}

			}

			bool minimized = wParam == SIZE_MINIMIZED;
			bool maximized = wParam == SIZE_MAXIMIZED || (handle->maximized && wParam != SIZE_RESTORED);

			if (handle->minimized != minimized) {
				notifyStateChange(minimized ? WindowState::Minimized : WindowState::Restored);
				handle->minimized = minimized;
			}

			if (handle->maximized != maximized) {
				notifyStateChange(maximized ? WindowState::Maximized : WindowState::Restored);
				handle->maximized = maximized;
			}

			return 0;

		}

		case WM_MOVE: {

			// i16 cast required because LOWORD/HIWORD returns an
			// unsigned value and windows can move to negative positions
			i32 x = static_cast<i16>(LOWORD(lParam));
			i32 y = static_cast<i16>(HIWORD(lParam));

			if (handle->moveFunction) {
				handle->moveFunction(window, x, y);
			}

			return 0;

		}

		case WM_GETMINMAXINFO: {

			auto* mmi = reinterpret_cast<MINMAXINFO*>(lParam);

			if (handle->minSize != Vec2ui(-1, -1)) {

				mmi->ptMinTrackSize.x = static_cast<LONG>(handle->minSize.x);
				mmi->ptMinTrackSize.y = static_cast<LONG>(handle->minSize.y);

			}

			if (handle->maxSize != Vec2ui(-1, -1)) {

				mmi->ptMaxTrackSize.x = static_cast<LONG>(handle->maxSize.x);
				mmi->ptMaxTrackSize.y = static_cast<LONG>(handle->maxSize.y);

			}

			return 0;

		}

		case WM_DROPFILES: {

			if (!handle->dropFunction) {
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

			handle->dropFunction(window, paths);

			DragFinish(hdrop);
			return 0;

		}

		case WM_SETFOCUS: {

			handle->focused = true;
			notifyStateChange(WindowState::Focused);
			return 0;

		}

		case WM_KILLFOCUS: {

			handle->focused = false;
			notifyStateChange(WindowState::Unfocused);
			return 0;

		}

		case WM_CLOSE: {

			handle->closeRequested = true;
			notifyStateChange(WindowState::CloseRequest);
			return 0;
		}

		case WM_DESTROY: {
			// DO NOTHING FOR NOW
			return 0;
		}

		default: break;

	}

	return {};

}

void WindowHandle::setWindowMessageHandler(const WindowMessageHandlerFunction& handler) {
	windowMessageHandlerFunction = handler;
}

void WindowHandle::setInputMessageHandler(const InputMessageHandlerFunction& handler) {
	inputMessageHandlerFunction = handler;
}