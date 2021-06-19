#include "input/keytrigger.h"
#include "util/assert.h"



void KeyTrigger::addKey(u32 key) {

	arc_assert(key != invalidKey, "Cannot use invalid key in key trigger");
	
	if (keyCount == maxTriggerKeys) {
		Log::warn("Input Trigger", "Tried to add key to trigger exceeding the key trigger max count");
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