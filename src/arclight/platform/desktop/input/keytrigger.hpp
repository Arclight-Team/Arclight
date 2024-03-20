/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 keytrigger.hpp
 */

#pragma once

#include "keydefs.hpp"
#include "common/assert.hpp"
#include "common/types.hpp"

#include <initializer_list>


class KeyTrigger {

public:

	constexpr static u32 maxTriggerKeys = 3;
	constexpr static u32 invalidKey = 0xFFFFFFFFu;

	constexpr KeyTrigger() : keyState(KeyState::Pressed), keyCount(0) {
		resetKeys();
	}

	constexpr KeyTrigger(const KeyTrigger& other, KeyState state) : keyState(state), keyCount(other.keyCount) {

		for (u32 i = 0; i < keyCount; i++) {
			this->keys[i] = other.keys[i];
		}

		for (u32 i = keyCount; i < maxTriggerKeys; i++) {
			this->keys[i] = invalidKey;
		}

	}

	constexpr KeyTrigger(const std::initializer_list<Key>& keys, KeyState state = KeyState::Pressed) : keyState(state), keyCount(keys.size()) {

		arc_assert(keys.size(), "Key trigger size cannot be 0");
		
		if (keys.size() > maxTriggerKeys) {
			arc_force_assert("Cannot construct a key trigger consisting of more than %d keys", maxTriggerKeys);
			keyCount = maxTriggerKeys;
		}

		for (u32 i = 0; i < keyCount; i++) {
			this->keys[i] = *(keys.begin() + i);
		}

		for (u32 i = keyCount; i < maxTriggerKeys; i++) {
			this->keys[i] = invalidKey;
		}

	}

	void addKey(u32 key);
	constexpr void resetKeys() {

		keyCount = 0;

		for (u32 i = 0; i < maxTriggerKeys; i++) {
			keys[i] = invalidKey;
		}

	}
	void setKeyState(KeyState state);

	u32 getKey(u32 index) const;
	u32 getKeyCount() const;
	KeyState getKeyState() const;

	constexpr bool operator==(const KeyTrigger& trigger) const {
		
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