/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 KeyTrigger.cpp
 */

#include "KeyTrigger.hpp"
#include "Common/Assert.hpp"



void KeyTrigger::addKey(u32 key) {

	arc_assert(key != invalidKey, "Cannot use invalid key in key trigger");
	
	if (keyCount == maxTriggerKeys) {
		LogW("Input Trigger") << "Tried to add key to trigger exceeding the key trigger max count";
		return;
	}

	keys[keyCount] = key;
	keyCount++;

}



void KeyTrigger::setKeyState(KeyState state) {
	keyState = state;
}



u32 KeyTrigger::getKey(u32 index) const {
	
	arc_assert(index < maxTriggerKeys, "Requested key index in trigger out of range");
	return keys[index];

}



u32 KeyTrigger::getKeyCount() const {
	return keyCount;
}



KeyState KeyTrigger::getKeyState() const {
	return keyState;
}