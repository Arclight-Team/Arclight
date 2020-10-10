#include "core/input/inputtrigger.h"
#include "util/assert.h"



InputTrigger::InputTrigger() : triggerType(InputTriggerType::Pressed), keyCount(0) {
	resetKeys();
}



void InputTrigger::addKey(u32 key) {

	arc_assert(key != invalidKey, "Cannot use invalid key in key trigger");
	
	if (keyCount == maxTriggerKeys) {
		Log::warn("Input Trigger", "Tried to add key to trigger exceeding the key trigger max count");
		return;
	}

	keys[keyCount] = key;
	keyCount++;

}



void InputTrigger::resetKeys() {

	keyCount = 0;

	for (u32 i = 0; i < maxTriggerKeys; i++) {
		keys[i] = invalidKey;
	}

}



void InputTrigger::setTriggerType(InputTriggerType type) {
	triggerType = type;
}



u32 InputTrigger::getKey(u32 index) const {
	
	arc_assert(index < maxTriggerKeys, "Requested key index in trigger out of range");
	return keys[index];

}



u32 InputTrigger::getKeyCount() const {
	return keyCount;
}



InputTriggerType InputTrigger::getTriggerType() const {
	return triggerType;
}