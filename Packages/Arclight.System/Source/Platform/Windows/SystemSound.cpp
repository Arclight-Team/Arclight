/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SystemSound.cpp
 */

#include "System/SystemSound.hpp"
#include "Common/Assert.hpp"
#include "Common/Win32.hpp"



void System::SystemSound::play(Sound sound) {

	switch (sound) {

		case Sound::Info:
			MessageBeep(MB_ICONINFORMATION);
			break;

		case Sound::Warning:
			MessageBeep(MB_ICONWARNING);
			break;

		case Sound::Error:
			MessageBeep(MB_ICONERROR);
			break;

		case Sound::Question:
			MessageBeep(MB_ICONQUESTION);
			break;

		default:
			arc_force_assert("Illegal sound parameter");

		case Sound::Ok:
			MessageBeep(MB_OK);
			break;

	}

}