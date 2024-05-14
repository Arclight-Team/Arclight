/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 KeyContext.hpp
 */

#pragma once

#include "Key.hpp"

#include <vector>



class KeyContext {

public:

	KeyContext();

	void resetAll();
	void resetEvents();

	void setKeyState(Key physicalKey, Key virtualKey, bool pressed);
	void clearEvent(Key key, bool physical);

	const std::vector<bool>& getKeyMap(bool physical) const;
	const std::vector<u32>& getEvents(bool physical) const;

private:

	std::vector<bool> physicalKeys;
	std::vector<bool> virtualKeys;
	std::vector<u32> physicalEvents;
	std::vector<u32> virtualEvents;

};