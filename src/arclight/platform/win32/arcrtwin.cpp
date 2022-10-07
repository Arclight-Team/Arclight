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
			LogE("Runtime") << "Failed to initialize CommCtrl";
			return false;
		}

		if (CoInitialize(nullptr)) {
			LogE("Runtime") << "Failed to initialize COM";
			return false;
		}

		if (!SetConsoleOutputCP(CP_UTF8)) {
			LogE("Runtime") << "Console failed to switch to Unicode";
			return false;
		}

		return true;

	}

}
