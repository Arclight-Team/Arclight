#pragma once

#include "input/keydefs.h"
#include "types.h"
#include <initializer_list>



class KeyTrigger {

public:

	constexpr static u32 maxTriggerKeys = 3;
	constexpr static u32 invalidKey = -1;

	KeyTrigger();
	KeyTrigger(std::initializer_list<Key> keys, KeyState state = KeyState::Pressed);

	void addKey(u32 key);
	void resetKeys();
	void setKeyState(KeyState state);

	u32 getKey(u32 index) const;
	u32 getKeyCount() const;
	KeyState getKeyState() const;

	inline bool operator==(const KeyTrigger& trigger) const {
		
		if (keyCount != trigger.keyCount || keyState != trigger.keyState) {
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

	Key keys[maxTriggerKeys];
	u32 keyCount;
	KeyState keyState;
	
};