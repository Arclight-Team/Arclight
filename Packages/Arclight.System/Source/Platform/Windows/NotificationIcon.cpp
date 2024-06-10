/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 NotificationIcon.cpp
 */

#include "System/Notification.hpp"

#include "Image/Image.hpp"



namespace System {

	void Notification::setIcon(const Image<Pixel::RGBA8>& icon) {
		setIconInternal(*handle, icon.getWidth(), icon.getHeight(), icon.getImageData(), true);
	}



	void Notification::setImage(const Image<Pixel::RGBA8>& icon) {
		setIconInternal(*handle, icon.getWidth(), icon.getHeight(), icon.getImageData(), false);
	}

}
