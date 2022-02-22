/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arcrtwin.cpp
 */

#include "util/log.hpp"

#include <Windows.h>
#include <CommCtrl.h>



namespace ArcRuntime {

	bool platformInit() {

		INITCOMMONCONTROLSEX commctrl;
		commctrl.dwSize = sizeof(commctrl);
		commctrl.dwICC = ICC_WIN95_CLASSES | ICC_USEREX_CLASSES | ICC_STANDARD_CLASSES | ICC_PAGESCROLLER_CLASS | ICC_NATIVEFNTCTL_CLASS | ICC_LINK_CLASS | ICC_INTERNET_CLASSES | ICC_DATE_CLASSES | ICC_COOL_CLASSES;

		if (!InitCommonControlsEx(&commctrl)) {
			Log::error("Runtime", "Failed to initialize CommCtrl");
			return false;
		}

		return true;

	}

}
