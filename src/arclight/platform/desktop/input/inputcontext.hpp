/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 inputcontext.hpp
 */

#pragma once

#include "keytrigger.hpp"
#include "inputhandler.hpp"

#include <unordered_map>
#include <optional>


class KeyEvent;
class CharEvent;
class CursorEvent;
class ScrollEvent;

class InputContext {

	struct State {

		inline State(bool disablePropagation = false) : disablePropagation(disablePropagation) {}

		bool disablePropagation;
		std::unordered_multimap<Key, KeyAction> keyLookup;
		std::vector<KeyAction> coActions;

	};

public:

	constexpr static u32 invalidState = -1;

	inline InputContext() : enabled(true), handler(nullptr), currentState(invalidState) {}
	~InputContext();

	InputContext(const InputContext& context) = delete;
	InputContext& operator=(const InputContext& context) = delete;
	InputContext(InputContext&& context) noexcept = default;
	InputContext& operator=(InputContext&& context) = default;

	void addState(u32 stateID, bool disablePropagation = false);
	void removeState(u32 stateID);
	void switchState(u32 stateID);
	bool stateAdded(u32 stateID) const;

	void addAction(KeyAction action, const KeyTrigger& trigger, bool continuous = false);
	void addBoundAction(KeyAction action, const KeyTrigger& boundTrigger, const KeyTrigger& defaultTrigger, bool continuous = false);
	void removeAction(KeyAction action);
	void clearActions();
	bool actionAdded(KeyAction action) const;

	void setBinding(KeyAction action, const KeyTrigger& binding);
	void restoreBinding(KeyAction action);
	void restoreAllBindings();
	const KeyTrigger& getActionBinding(KeyAction action) const;
	const KeyTrigger& getActionDefaultBinding(KeyAction action) const;
	bool isActionContinuous(KeyAction action) const;

	void registerAction(u32 stateID, KeyAction action);
	void unregisterAction(u32 stateID, KeyAction action);
	void unregisterActionGroup(KeyAction action);
	void unregisterStateActions(u32 stateID);
	void unregisterAllActions();
	bool actionRegistered(u32 stateID, KeyAction action) const;

	void disable();
	void enable();

	bool onKeyEvent(const KeyEvent& event, const std::vector<KeyState>& keyStates);
	bool onCharEvent(const CharEvent& event);
	bool onCursorEvent(const CursorEvent& event);
	bool onScrollEvent(const ScrollEvent& event);
	bool onContinuousEvent(u32 ticks, const std::vector<KeyState>& keyStates, std::vector<u32>& eventCounts);

	void linkHandler(InputHandler& handler);
	void unlinkHandler();

	u32 getCurrentStateID() const;
	bool propagationDisabled() const;

private:

	bool triggerCompare(const std::vector<KeyState>& keyStates, const KeyTrigger& trigger, const KeyEvent& event) const;

	bool enabled;
	u32 currentState;
	InputHandler* handler;
	std::unordered_map<u32, State> inputStates;
	std::unordered_map<KeyAction, std::pair<KeyTrigger, bool>> actionBindings;
	std::unordered_map<KeyAction, KeyTrigger> defaultBindings;

};