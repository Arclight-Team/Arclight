/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 systemsound.hpp
 */

#pragma once

#include <string>



namespace OS::SystemSound {

	enum class Sound {
		Info,
		Warning,
		Error,
		Question,
		Ok
	};

	void play(Sound sound);

};