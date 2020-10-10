#pragma once

#include "types.h"


typedef u32 InputAction;


enum class InputTriggerType {
	Pressed,
	Released
};


struct InputTrigger {

	constexpr static u32 maxTriggerKeys = 3;
	constexpr static u32 invalidKey = -1;

	InputTrigger();

	void addKey(u32 key);
	void resetKeys();
	void setTriggerType(InputTriggerType type);

	u32 getKey(u32 index) const;
	u32 getKeyCount() const;
	InputTriggerType getTriggerType() const;

	inline bool operator==(const InputTrigger& trigger) const {
		
		if (keyCount != trigger.keyCount) {
			return false;
		}

		if (triggerType != trigger.triggerType) {
			return false;
		}

		for (u32 i = 0; i < keyCount; i++) {

			bool found = false;

			for (u32 j = 0; j < keyCount; j++) {

				if (keys[i] == trigger.keys[j]) {
					found = true;
					break;
				}

			}

			if (!found) {
				return false;
			}

		}

		return true;

	}

private:

	u32 keys[maxTriggerKeys];
	u32 keyCount;
	InputTriggerType triggerType;

};