/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 inputcontext.cpp
 */

#include "inputcontext.hpp"
#include "util/log.hpp"
#include "util/assert.hpp"

#include <algorithm>




InputContext::InputContext() : enabled(true), handler(nullptr), currentState(0) {
	addState(0, false);
}

InputContext::~InputContext() {
	unlinkHandler();
}



void InputContext::addState(u32 stateID, bool disablePropagation) {

	if (stateAdded(stateID)) {
		Log::warn("Input Context", "State with ID %d does already exist", stateID);
		return;
	}

	inputStates.emplace(stateID, disablePropagation);

}



void InputContext::removeState(u32 stateID) {

	if (!stateAdded(stateID)) {
		Log::warn("Input Context", "State with ID %d doesn't exist", stateID);
		return;
	}

	inputStates.erase(stateID);

}



void InputContext::switchState(u32 stateID) {

	if (stateAdded(stateID)) {
		currentState = stateID;
	} else {
		Log::warn("Input Context", "State with ID %d doesn't exist", stateID);
	}

}



bool InputContext::stateAdded(u32 stateID) const {
	return inputStates.contains(stateID);
}



void InputContext::addAction(KeyAction action, const KeyTrigger& trigger, bool continuous) {

	arc_assert(!actionAdded(action), "Action %d already added to context", action);

	actionBindings[action] = std::make_pair(trigger, continuous);
	defaultBindings[action] = trigger;

}



void InputContext::addBoundAction(KeyAction action, const KeyTrigger& boundTrigger, const KeyTrigger& defaultTrigger, bool continuous) {

	arc_assert(!actionAdded(action), "Action %d already added to context", action);

	actionBindings[action] = std::make_pair(boundTrigger, continuous);
	defaultBindings[action] = defaultTrigger;

}



void InputContext::removeAction(KeyAction action) {

	arc_assert(actionAdded(action), "Action %d not added to context", action);

	unregisterStateActions(action);
	actionBindings.erase(action);
	defaultBindings.erase(action);

}



void InputContext::clearActions() {

	unregisterAllActions();
	actionBindings.clear();
	defaultBindings.clear();

}



bool InputContext::actionAdded(KeyAction action) const {
	return actionBindings.contains(action);
}



void InputContext::addRegisteredAction(u32 stateID, KeyAction action, const KeyTrigger& trigger, bool continuous) {

	addAction(action, trigger, continuous);
	registerAction(stateID, action);

}



void InputContext::addRegisteredBoundAction(u32 stateID, KeyAction action, const KeyTrigger& boundTrigger, const KeyTrigger& defaultTrigger, bool continuous) {

	addBoundAction(action, boundTrigger, defaultTrigger, continuous);
	registerAction(stateID, action);

}



void InputContext::setBinding(KeyAction action, const KeyTrigger& binding) {

	if (!actionAdded(action)) {
		Log::warn("Input Context", "Cannot set action binding without a default action trigger");
		return;
	}

	std::vector<u32> boundStates;

	for (const auto& [stateID, state] : inputStates) {

		if (actionRegistered(stateID, action)) {

			unregisterAction(stateID, action);
			boundStates.push_back(stateID);

		}

	}

	actionBindings[action].first = binding;

	for (u32 stateID : boundStates) {
		registerAction(stateID, action);
	}

}



void InputContext::restoreBinding(KeyAction action) {

	if (!actionAdded(action)) {
		Log::warn("Input Context", "Cannot restore action binding without a default action trigger");
		return;
	}

	setBinding(action, defaultBindings[action]);

}



void InputContext::restoreAllBindings() {

	for (const auto& [action, trigger] : defaultBindings) {
		restoreBinding(action);
	}

}



const KeyTrigger& InputContext::getActionBinding(KeyAction action) const {

	arc_assert(actionAdded(action), "Action %d not added to context", action);

	return actionBindings.at(action).first;

}



const KeyTrigger& InputContext::getActionDefaultBinding(KeyAction action) const {

	arc_assert(actionAdded(action), "Action %d not added to context", action);

	return defaultBindings.at(action);

}



bool InputContext::isActionContinuous(KeyAction action) const {

	arc_assert(actionAdded(action), "Action bindings don't contain action %d", action);

	return actionBindings.at(action).second;

}



void InputContext::registerAction(u32 stateID, KeyAction action) {

	arc_assert(stateAdded(stateID), "State %d not defined for input context while registering key action %d", stateID, action);
	arc_assert(actionAdded(action), "Action bindings don't contain action %d", action);

	const KeyTrigger& trigger = getActionBinding(action);

	if (isActionContinuous(action)) {

		auto& coActions = inputStates[stateID].coActions;

		for (auto it = coActions.begin(); it != coActions.end(); it++) {

			if (getActionBinding(*it).getKeyCount() <= trigger.getKeyCount()) {
				coActions.insert(it, action);
				return;
			}

		}

		coActions.insert(coActions.end(), action);

	} else {

		auto& keyMap = inputStates[stateID].keyLookup;

		for (u32 i = 0; i < trigger.getKeyCount(); i++) {

			const auto& actions = keyMap.equal_range(trigger.getKey(i));

			for (auto it = actions.first; it != actions.second; it++) {

				if (actionBindings[it->second].first.getKeyCount() <= trigger.getKeyCount()) {
					keyMap.emplace_hint(it, trigger.getKey(i), action);
					return;
				}

			}

			keyMap.emplace_hint(actions.second, trigger.getKey(i), action);

		}

	}

}



void InputContext::unregisterAction(u32 stateID, KeyAction action) {

	arc_assert(stateAdded(stateID), "State %d not defined for input context while unregistering key action %d", stateID, action);
	arc_assert(actionAdded(action), "Action bindings don't contain action %d", action);

	if (isActionContinuous(action)) {

		auto& coActions = inputStates[stateID].coActions;

		for (u32 i = 0; i < coActions.size(); i++) {

			if (coActions[i] == action) {
				coActions.erase(coActions.begin() + i);
				return;
			}

		}

	} else {

		auto& keyMap = inputStates[stateID].keyLookup;
		const KeyTrigger& trigger = getActionBinding(action);

		for (u32 i = 0; i < trigger.getKeyCount(); i++) {

			const auto& actions = keyMap.equal_range(trigger.getKey(i));

			for (auto it = actions.first; it != actions.second; it++) {

				if (it->second == action) {
					keyMap.erase(it);
					break;
				}

			}

		}

	}

}



void InputContext::unregisterActionGroup(KeyAction action) {

	for (auto& state : inputStates) {
		unregisterAction(state.first, action);
	}

}



void InputContext::unregisterStateActions(u32 stateID) {

	arc_assert(stateAdded(stateID), "State %d not defined for input context while unregistering all actions", stateID);

	auto& keyMap = inputStates[stateID].keyLookup;
	auto& coActions = inputStates[stateID].coActions;
	
	keyMap.clear();
	coActions.clear();

}



void InputContext::unregisterAllActions() {

	for (auto& state : inputStates) {
		unregisterStateActions(state.first);
	}

}



bool InputContext::actionRegistered(u32 stateID, KeyAction action) const {

	arc_assert(stateAdded(stateID), "State %d not defined for input context while unregistering all action", stateID);
	arc_assert(actionAdded(action), "Action bindings don't contain action %d", action);

	if (isActionContinuous(action)) {

		auto& coActions = inputStates.at(stateID).coActions;

		for (u32 i = 0; i < coActions.size(); i++) {

			if (coActions[i] == action) {
				return true;
			}

		}

	} else {

		Key key = getActionBinding(action).getKey(0);
		auto& keyMap = inputStates.at(stateID).keyLookup;
		const auto& actions = keyMap.equal_range(key);

		for (auto it = actions.first; it != actions.second; it++) {

			if (it->second == action) {
				return true;
			}

		}

	}

	return false;

}



void InputContext::disable() {
	enabled = false;
}



void InputContext::enable() {
	enabled = true;
}



bool InputContext::onKeyEvent(const KeyEvent& event, const std::vector<KeyState>& keyStates) {

	if (!enabled || !handler) {
		return propagationDisabled();
	}

	bool consumed = false;
	Key key = event.getKey();
	KeyState state = event.getKeyState();

	if (handler->actionListener) {

		auto& keyMap = inputStates[currentState].keyLookup;
		const auto& actions = keyMap.equal_range(key);
	
		for (auto it = actions.first; it != actions.second; it++) {

			const KeyTrigger& trigger = getActionBinding(it->second);

			if(triggerCompare(keyStates, trigger, event)){
			
				if (handler->actionListener(it->second)) {
					consumed = true;
					break;
				}

			}

		}

	}

	if (handler->keyListener) {
		consumed |= handler->keyListener(key, state);
	}

	return consumed || propagationDisabled();

}



bool InputContext::onCharEvent(const CharEvent& event) {

	if (!enabled || !handler || !handler->charListener) {
		return propagationDisabled();
	}

	return handler->charListener(event.getChar()) || propagationDisabled();

}



bool InputContext::onCursorEvent(const CursorEvent& event) {

	if (!enabled || !handler || !handler->cursorListener) {
		return propagationDisabled();
	}

	return handler->cursorListener(event.getX(), event.getY()) || propagationDisabled();

}



bool InputContext::onScrollEvent(const ScrollEvent& event) {

	if (!enabled || !handler || !handler->scrollListener) {
		return propagationDisabled();
	}

	return handler->scrollListener(event.scrollX(), event.scrollY()) || propagationDisabled();

}



bool InputContext::onContinuousEvent(u32 ticks, const std::vector<KeyState>& keyStates, std::vector<u32>& eventCounts) {

	if (!enabled || !handler || !handler->coActionListener) {
		return propagationDisabled();
	}

	for (KeyAction action : inputStates[currentState].coActions) {

		u32 combinedEventCount = -1;
		const KeyTrigger& trigger = getActionBinding(action);

		for (u32 i = 0; i < trigger.getKeyCount(); i++) {

			Key key = trigger.getKey(i);
			u32 eventCount = 0;

			if (keyStates[key] == trigger.getKeyState() && eventCounts[key] == 0) {
				eventCount = ticks;
			} else {
				eventCount = (eventCounts[key] + (keyStates[key] == trigger.getKeyState())) / 2;
			}

			combinedEventCount = std::min(combinedEventCount, eventCount);

		}

		bool result = false;

		for (u32 i = 0; i < combinedEventCount; i++) {
			result |= handler->coActionListener(action, 1);
		}

		if (result) {
				
			for (u32 i = 0; i < trigger.getKeyCount(); i++) {
				eventCounts[trigger.getKey(i)] = 0;
			}

		}

	}

	return propagationDisabled();

}



void InputContext::linkHandler(InputHandler& handler) {

	unlinkHandler();
	this->handler = &handler;
	this->handler->context = this;

}



void InputContext::unlinkHandler() {

	if (handler) {
		handler->context = nullptr;
		handler = nullptr;
	}

}



u32 InputContext::getCurrentStateID() const {
	return currentState;
}



bool InputContext::propagationDisabled() const {
	return currentState == -1 ? false : inputStates.at(currentState).disablePropagation;
}



bool InputContext::triggerCompare(const std::vector<KeyState>& keyStates, const KeyTrigger& trigger, const KeyEvent& event) const {

	if (trigger.getKeyState() != event.getKeyState()) {
		return false;
	}

	for (u32 i = 0; i < trigger.getKeyCount(); i++) {

		Key key = trigger.getKey(i);

		if (keyStates[key] != trigger.getKeyState()) {
			return false;
		}

	}

	return true;

}