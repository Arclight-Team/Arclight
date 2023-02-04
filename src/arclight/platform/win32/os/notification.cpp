/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 notification.cpp
 */

#include "notification.hpp"
#include "common.hpp"
#include "locale/unicode.hpp"
#include "util/destructionguard.hpp"

#include <unordered_set>

#include <Windows.h>



namespace OS {

	class NotificationHandle {

	public:

		NOTIFYICONDATAW data {};
		bool active;

		NotificationHandle() : active(false) {}

		~NotificationHandle() {

			if (data.hIcon) {
				DestroyIcon(data.hIcon);
			}

			if (data.hBalloonIcon) {
				DestroyIcon(data.hBalloonIcon);
			}

		}

	};



	static std::unordered_set<u64> activeNotifications;
	static u64 activeID = 0;



	static void setIconInternal(NotificationHandle& handle, const Image<Pixel::RGBA8>& icon, bool isIcon) {

		ICONINFO iconInfo {true, 0, 0, nullptr, nullptr };

		DestructionGuard guard([&]() {

			if (iconInfo.hbmColor) {
				DeleteObject(iconInfo.hbmColor);
			}

			if (iconInfo.hbmMask) {
				DeleteObject(iconInfo.hbmMask);
			}

		});

		Image<Pixel::BGRA8> iIcon = icon.convert<Pixel::BGRA8>();
		iIcon.flipY();

		iconInfo.hbmColor = CreateBitmap(iIcon.getWidth(), iIcon.getHeight(), 1, 32, iIcon.getImageBuffer().data());
		iconInfo.hbmMask = CreateBitmap(iIcon.getWidth(), iIcon.getHeight(), 1, 1, nullptr);

		if (!iconInfo.hbmColor || !iconInfo.hbmMask) {
			LogE("Notification") << "Failed to create notification icon";
			return;
		}

		HICON& hIcon = isIcon ? handle.data.hIcon : handle.data.hBalloonIcon;

		if (hIcon) {
			DestroyIcon(hIcon);
		}

		hIcon = CreateIconIndirect(&iconInfo);

		handle.data.dwInfoFlags = NIIF_USER;

	}




	Notification::Notification() : options(Options::None) {
		createHandle();
	}

	Notification::~Notification() = default;



	Notification::Options Notification::getOptions() const {
		return options;
	}



	void Notification::setTitle(const std::string& title) {

		std::wstring str = OS::String::toUTF16(title);

		if (str.size() > 63) {
			str.resize(63);
		}

		std::copy(str.begin(), str.end(), handle->data.szInfoTitle);
		handle->data.szInfoTitle[str.size()] = 0;

	}



	void Notification::setText(const std::string& text) {

		std::wstring str = OS::String::toUTF16(text);

		if (str.size() > 255) {
			str.resize(255);
		}

		std::copy(str.begin(), str.end(), handle->data.szInfo);
		handle->data.szInfo[str.size()] = 0;

	}



	void Notification::setTooltip(const std::string& tooltip) {

		std::wstring str = OS::String::toUTF16(tooltip);

		if (str.size() > 127) {
			str.resize(127);
		}

		std::copy(str.begin(), str.end(), handle->data.szTip);
		handle->data.szTip[str.size()] = 0;

	}



	void Notification::setStandardIcon(const std::string& name) {

		if (name == "Info") {
			handle->data.dwInfoFlags = NIIF_INFO;
		} else if (name == "Warning") {
			handle->data.dwInfoFlags = NIIF_WARNING;
		} else if (name == "Error") {
			handle->data.dwInfoFlags = NIIF_ERROR;
		} else {
			handle->data.dwInfoFlags = NIIF_NONE;
		}

	}



	void Notification::setIcon(const Image<Pixel::RGBA8>& icon) {
		setIconInternal(*handle, icon, true);
	}



	void Notification::setImage(const Image<Pixel::RGBA8>& icon) {
		setIconInternal(*handle, icon, false);
	}



	void Notification::removeIcon() {

		handle->data.dwInfoFlags = NIIF_NONE;

		HICON& icon = handle->data.hIcon;

		if (icon) {

			DestroyIcon(icon);
			icon = nullptr;

		}

	}



	void Notification::removeImage() {

		HICON& icon = handle->data.hBalloonIcon;

		if (icon) {

			DestroyIcon(icon);
			icon = nullptr;

		}

	}



	void Notification::setOptions(Options options) {
		this->options = options;
	}



	void Notification::show() {

		NOTIFYICONDATAW& data = handle->data;
		data.cbSize = sizeof(data);
		data.uFlags = NIF_TIP | NIF_INFO;

		if (data.hIcon) {
			data.uFlags |= NIF_ICON;
		}

		if ((options & Hidden) == Hidden) {

			data.uFlags |= NIF_STATE;
			data.dwState = NIS_HIDDEN;
			data.dwStateMask = NIS_HIDDEN;

		}

		data.uVersion = NOTIFYICON_VERSION_4;
		data.hWnd = nullptr;

		data.dwInfoFlags |= NIIF_LARGE_ICON | NIIF_RESPECT_QUIET_TIME;

		if ((options & NoSound) == NoSound) {
			data.dwInfoFlags |= NIIF_NOSOUND;
		}

		if (!handle->active) {

			handle->active = true;
			activeNotifications.insert(activeID);
			data.uID = activeID++;

			Shell_NotifyIconW(NIM_ADD, &data);
			Shell_NotifyIconW(NIM_SETVERSION, &data);

		} else {

			Shell_NotifyIconW(NIM_MODIFY, &data);

		}

	}



	void Notification::remove() {

		if (handle->active) {

			Shell_NotifyIconW(NIM_DELETE, &handle->data);
			activeNotifications.erase(handle->data.uID);
			handle->active = false;

		}

	}



	Notification Notification::post(const std::string& title, const std::string& text, const std::string& tooltip, const std::string& icon, Options options) {

		Notification notification;
		notification.setTitle(title);
		notification.setText(text);
		notification.setTooltip(tooltip);
		notification.setStandardIcon(icon);
		notification.setOptions(options);
		notification.show();

		return notification;

	}



	Notification Notification::post(const std::string& title, const std::string& text, const std::string& tooltip, const Image<Pixel::RGBA8>& icon, Options options) {

		Notification notification;
		notification.setTitle(title);
		notification.setText(text);
		notification.setTooltip(tooltip);
		notification.setIcon(icon);
		notification.setOptions(options);
		notification.show();

		return notification;

	}



	Notification Notification::post(const std::string& title, const std::string& text, const std::string& tooltip, const Image<Pixel::RGBA8>& icon, const Image<Pixel::RGBA8>& image, Options options) {

		Notification notification;
		notification.setTitle(title);
		notification.setText(text);
		notification.setTooltip(tooltip);
		notification.setIcon(icon);
		notification.setImage(image);
		notification.setOptions(options);
		notification.show();

		return notification;

	}



	void Notification::purgeAll() {

		NOTIFYICONDATAW data {};

		for (u64 id : activeNotifications) {

			data.uID = id;
			Shell_NotifyIconW(NIM_DELETE, &data);

		}

	}



	void Notification::createHandle() {
		handle = std::make_unique<NotificationHandle>();
	}

}