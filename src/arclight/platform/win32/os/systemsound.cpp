/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 systemsound.cpp
 */

#include "systemsound.hpp"
#include "util/assert.hpp"

#include <Windows.h>



void OS::SystemSound::play(Sound sound) {

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