#include "core/input/keytrigger.h"
#include "util/assert.h"



KeyTrigger::KeyTrigger() : keyState(KeyState::Pressed), keyCount(0) {
	resetKeys();
}


KeyTrigger::KeyTrigger(std::initializer_list<Key> keys, KeyState state) : keyState(state), keyCount(keys.size()) {

	arc_assert(keys.size(), "Key trigger size cannot be 0");

	for (u32 i = 0; i < keyCount; i++) {
		this->keys[i] = *(keys.begin() + i);
	}

	for (u32 i = keyCount; i < maxTriggerKeys; i++) {
		this->keys[i] = invalidKey;
	}

}


void KeyTrigger::addKey(u32 key) {

	arc_assert(key != invalidKey, "Cannot use invalid key in key trigger");
	
	if (keyCount == maxTriggerKeys) {
		Log::warn("Input Trigger", "Tried to add key to trigger exceeding the key trigger max count");
		return;
	}

	keys[keyCount] = key;
	keyCount++;

}



void KeyTrigger::resetKeys() {

	keyCount = 0;

	for (u32 i = 0; i < maxTriggerKeys; i++) {
		keys[i] = invalidKey;
	}

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