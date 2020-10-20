#include "core/input/inputcontext.h"
#include "util/log.h"
#include "util/assert.h"



InputContext::~InputContext() {
	unlinkHandler();
}



void InputContext::addState(u32 stateID, bool disablePropagation) {

	if (stateAdded(stateID)) {
		Log::warn("Input Context", "State with ID %d does already exist", stateID);
		return;
	}

	inputStates.emplace(stateID, disablePropagation);

	if (currentState == invalidState) {
		switchState(stateID);
	}

}



void InputContext::removeState(u32 stateID) {

	if (!stateAdded(stateID)) {
		Log::warn("Input Context", "State with ID %d doesn't exist", stateID);
		return;
	}

	inputStates.erase(stateID);

}



void InputContext::switchState(u32 stateID) {

	arc_assert(stateAdded(stateID), "State with ID %d doesn't exist", stateID);

	currentState = stateID;

}



bool InputContext::stateAdded(u32 stateID) const {
	return inputStates.contains(stateID);
}



void InputContext::addBoundAction(KeyAction action, const KeyTrigger& boundTrigger, const KeyTrigger& defaultTrigger) {

	arc_assert(!actionAdded(action), "Action %d already added to context", action);

	actionBindings[action] = boundTrigger;
	defaultBindings[action] = defaultTrigger;

}



void InputContext::addAction(KeyAction action, const KeyTrigger& trigger) {

	arc_assert(!actionAdded(action), "Action %d already added to context", action);

	actionBindings[action] = trigger;
	defaultBindings[action] = trigger;

}



void InputContext::removeAction(KeyAction action) {

	arc_assert(actionAdded(action), "Action %d not added to context", action);

	unregisterAction(action);
	actionBindings.erase(action);
	defaultBindings.erase(action);

}



void InputContext::clearActions() {

	unregisterActions();
	actionBindings.clear();
	defaultBindings.clear();

}



bool InputContext::actionAdded(KeyAction action) const {
	return actionBindings.contains(action);
}



void InputContext::setBinding(KeyAction action, const KeyTrigger& binding) {

	if (!actionAdded(action)) {
		Log::warn("Input Context", "Cannot set action binding without a default action trigger");
		return;
	}

	for (const auto& [stateID, state] : inputStates) {

		if (actionRegistered(stateID, action)) {

			unregisterAction(stateID, action);
			actionBindings[action] = binding;
			registerAction(stateID, action);

		}

	}

}



void InputContext::restoreBinding(KeyAction action) {

	if (!actionAdded(action)) {
		Log::warn("Input Context", "Cannot restore action binding without a default action trigger");
		return;
	}

	setBinding(action, defaultBindings[action]);

}



void InputContext::restoreBindings() {

	for (const auto& [action, trigger] : defaultBindings) {
		restoreBinding(action);
	}

}



KeyTrigger InputContext::getActionBinding(KeyAction action) const {

	arc_assert(actionAdded(action), "Action %d not added to context", action);

	return actionBindings.at(action);

}



KeyTrigger InputContext::getActionDefaultBinding(KeyAction action) const {

	arc_assert(actionAdded(action), "Action %d not added to context", action);

	return defaultBindings.at(action);

}



void InputContext::registerAction(u32 stateID, KeyAction action) {

	arc_assert(stateAdded(stateID), "State %d not defined for input context while registering key action %d", stateID, action);
	arc_assert(actionAdded(action), "Action bindings don't contain action %d", action);

	auto& keyMap = inputStates[stateID].keyLookup;
	const KeyTrigger& trigger = actionBindings[action];

	for (u32 i = 0; i < trigger.getKeyCount(); i++) {

		const auto& actions = keyMap.equal_range(trigger.getKey(i));

		for (auto it = actions.first; it != actions.second; it++) {

			if (actionBindings[it->second].getKeyCount() <= trigger.getKeyCount()) {
				keyMap.emplace_hint(it, trigger.getKey(i), action);
				return;
			}

		}

		keyMap.emplace_hint(actions.second, trigger.getKey(i), action);

	}

}



void InputContext::unregisterAction(u32 stateID, KeyAction action) {

	arc_assert(stateAdded(stateID), "State %d not defined for input context while unregistering key action %d", stateID, action);
	arc_assert(actionAdded(action), "Action bindings don't contain action %d", action);

	auto& keyMap = inputStates[stateID].keyLookup;
	const KeyTrigger& trigger = actionBindings[action];

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



void InputContext::unregisterAction(KeyAction action) {

	for (auto& state : inputStates) {
		unregisterAction(state.first, action);
	}

}



void InputContext::unregisterActions(u32 stateID) {

	arc_assert(stateAdded(stateID), "State %d not defined for input context while unregistering all actions", stateID);

	auto& keyMap = inputStates[stateID].keyLookup;
	keyMap.clear();

}



void InputContext::unregisterActions() {

	for (auto& state : inputStates) {
		unregisterActions(state.first);
	}

}



bool InputContext::actionRegistered(u32 stateID, KeyAction action) const {

	arc_assert(stateAdded(stateID), "State %d not defined for input context while unregistering all action", stateID);
	arc_assert(actionAdded(action), "Action bindings don't contain action %d", action);

	Key key = actionBindings.at(action).getKey(0);
	auto& keyMap = inputStates.at(stateID).keyLookup;
	const auto& actions = keyMap.equal_range(key);

	for (auto it = actions.first; it != actions.second; it++) {

		if (it->second == action) {
			return true;
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

			const KeyTrigger& trigger = actionBindings[it->second];

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
	return inputStates.at(currentState).disablePropagation;
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