/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 systemsound.cpp
 */

#include "systemsound.hpp"

#include <Windows.h>



void SystemSound::play(const std::string& sound) {

	if (sound == "Info") {
		MessageBeep(MB_ICONINFORMATION);
	} else if (sound == "Warning") {
		MessageBeep(MB_ICONWARNING);
	} else if (sound == "Error") {
		MessageBeep(MB_ICONERROR);
	} else if (sound == "Question") {
		MessageBeep(MB_ICONQUESTION);
	} else if (sound == "Ok") {
		MessageBeep(MB_OK);
	}

}