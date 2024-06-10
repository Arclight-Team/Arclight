/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 KeyContext.cpp
 */

#include "Desktop/Input/KeyContext.hpp"
#include "Desktop/Input/KeyConversion.hpp"



KeyContext::KeyContext() : modifiers(0) {

	physicalKeys.resize(KeyConversion::physicalKeyCount() * 2, false);
	virtualKeys.resize(KeyConversion::virtualKeyCount() * 2, false);

	physicalEvents.resize(physicalKeys.size(), 0);
	virtualEvents.resize(virtualKeys.size(), 0);

}



void KeyContext::resetAll() {

	modifiers = 0;
	std::fill(physicalKeys.begin(), physicalKeys.end(), false);
	std::fill(virtualKeys.begin(), virtualKeys.end(), false);

	resetEvents();

}



void KeyContext::resetEvents() {

	std::fill(physicalEvents.begin(), physicalEvents.end(), 0);
	std::fill(virtualEvents.begin(), virtualEvents.end(), 0);

}



void KeyContext::setKeyState(Key physicalKey, Key virtualKey, bool pressed) {

	if (KeyConversion::isVirtualModifierKey(virtualKey)) {

		u32 mod = KeyConversion::modifierFlag(virtualKey);

		if (pressed) {
			modifiers |= mod;
		} else {
			modifiers &= ~mod;
		}

	}

	physicalKeys[physicalKey] = pressed;
	virtualKeys[virtualKey] = pressed;

	physicalEvents[physicalKey]++;
	virtualEvents[virtualKey]++;

}



void KeyContext::clearEvent(Key key, bool physical) {
	(physical ? physicalEvents[key] : virtualEvents[key]) = 0;
}



u32 KeyContext::getModifierState() const {
	return modifiers;
}



u32 KeyContext::getModifierEventCount(u32 modifier) const {

	switch (modifier) {

		case KeyModifier::Shift:	return virtualEvents[VirtualKey::LeftShift]		+ virtualEvents[VirtualKey::RightShift];
		case KeyModifier::Control:	return virtualEvents[VirtualKey::LeftControl]	+ virtualEvents[VirtualKey::RightControl];
		case KeyModifier::Super:	return virtualEvents[VirtualKey::LeftSuper]		+ virtualEvents[VirtualKey::RightSuper];
		case KeyModifier::Alt:		return virtualEvents[VirtualKey::LeftAlt]		+ virtualEvents[VirtualKey::RightAlt];
		default:	return 0;

	}

}



const std::vector<bool>& KeyContext::getKeyMap(bool physical) const {
	return physical ? physicalKeys : virtualKeys;
}

const std::vector<u32>& KeyContext::getEvents(bool physical) const {
	return physical ? physicalEvents : virtualEvents;
}