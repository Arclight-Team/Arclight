/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 KeyContext.cpp
 */

#include "KeyContext.hpp"
#include "KeyConversion.hpp"



KeyContext::KeyContext() {

	physicalKeys.resize(KeyConversion::physicalKeyCount() * 2, false);
	virtualKeys.resize(KeyConversion::virtualKeyCount() * 2, false);

	physicalEvents.resize(physicalKeys.size(), 0);
	virtualEvents.resize(virtualKeys.size(), 0);

}



void KeyContext::resetAll() {

	std::fill(physicalKeys.begin(), physicalKeys.end(), false);
	std::fill(virtualKeys.begin(), virtualKeys.end(), false);

	resetEvents();

}



void KeyContext::resetEvents() {

	std::fill(physicalEvents.begin(), physicalEvents.end(), 0);
	std::fill(virtualEvents.begin(), virtualEvents.end(), 0);

}



void KeyContext::setKeyState(Key physicalKey, Key virtualKey, bool pressed) {

	physicalKeys[physicalKey] = pressed;
	virtualKeys[virtualKey] = pressed;

	physicalEvents[physicalKey]++;
	virtualEvents[virtualKey]++;

}



void KeyContext::clearEvent(Key key, bool physical) {
	(physical ? physicalEvents[key] : virtualEvents[key]) = 0;
}



const std::vector<bool>& KeyContext::getKeyMap(bool physical) const {
	return physical ? physicalKeys : virtualKeys;
}

const std::vector<u32>& KeyContext::getEvents(bool physical) const {
	return physical ? physicalEvents : virtualEvents;
}