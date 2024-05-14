/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 KeyTrigger.hpp
 */

#pragma once

#include "Key.hpp"
#include "Common/Types.hpp"

#include <span>
#include <initializer_list>



class KeyTrigger {

public:

	enum class Type {
		Physical,
		Virtual
	};

	constexpr static u32 MaxTriggerKeys = 3;

	KeyTrigger();
	KeyTrigger(const std::initializer_list<Key>& keys, KeyState state = KeyState::Pressed, u32 mods = KeyModifier::None, Type type = Type::Physical);
	explicit KeyTrigger(std::span<const Key> keys, KeyState state = KeyState::Pressed, u32 mods = KeyModifier::None, Type type = Type::Physical);

	void addKey(Key key);
	void resetKeys();
	void setKeyState(KeyState state);
	void setModifiers(u32 mods);
	void setPhysical();
	void setVirtual();

	Key getKey(u32 index) const;
	u32 getKeyCount() const;
	KeyState getKeyState() const;
	u32 getModifiers() const;
	bool isPhysical() const;
	bool isVirtual() const;

	bool operator==(const KeyTrigger& trigger) const;
	std::strong_ordering operator<=>(const KeyTrigger& trigger) const;

private:

	void sortKeys();

	Key keys[MaxTriggerKeys];
	u32 keyCount;
	KeyState keyState;
	u32 modifiers;
	Type type;
	
};