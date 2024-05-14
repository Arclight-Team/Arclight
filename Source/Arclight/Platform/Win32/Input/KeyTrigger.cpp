/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 KeyTrigger.cpp
 */

#include "KeyTrigger.hpp"
#include "Common/Assert.hpp"



KeyTrigger::KeyTrigger() : keyState(KeyState::Pressed), keyCount(0), modifiers(0), type(Type::Physical) {
	resetKeys();
}

KeyTrigger::KeyTrigger(const std::initializer_list<Key>& keys, KeyState state, u32 mods, Type type)
	: KeyTrigger(std::span{keys}, state, mods, type) {}

KeyTrigger::KeyTrigger(std::span<const Key> keys, KeyState state, u32 mods, Type type) : keyState(state), keyCount(keys.size()), modifiers(mods), type(type) {

	arc_assert(keys.size(), "Key trigger size cannot be 0");

	if (keys.size() > MaxTriggerKeys) {
		arc_force_assert("Cannot construct a key trigger consisting of more than %d keys", MaxTriggerKeys);
		keyCount = MaxTriggerKeys;
	}

	for (u32 i = 0; i < keyCount; i++) {
		this->keys[i] = keys[i];
	}

}



void KeyTrigger::addKey(Key key) {

	if (keyCount == MaxTriggerKeys) {
		LogW("Input Trigger") << "Tried to add key to trigger exceeding the key trigger max count";
		return;
	}

	keys[keyCount] = key;
	keyCount++;

}



void KeyTrigger::resetKeys() {
	keyCount = 0;
}



void KeyTrigger::setKeyState(KeyState state) {
	keyState = state;
}



void KeyTrigger::setModifiers(u32 mods) {
	modifiers = mods;
}



void KeyTrigger::setPhysical() {
	type = Type::Physical;
}



void KeyTrigger::setVirtual() {
	type = Type::Virtual;
}



Key KeyTrigger::getKey(u32 index) const {
	
	arc_assert(index < MaxTriggerKeys, "Requested key index in trigger out of range");
	return keys[index];

}



u32 KeyTrigger::getKeyCount() const {
	return keyCount;
}



KeyState KeyTrigger::getKeyState() const {
	return keyState;
}



u32 KeyTrigger::getModifiers() const {
	return modifiers;
}



bool KeyTrigger::isPhysical() const {
	return type == Type::Physical;
}



bool KeyTrigger::isVirtual() const {
	return type == Type::Virtual;
}



bool KeyTrigger::operator==(const KeyTrigger& trigger) const {

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