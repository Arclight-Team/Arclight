#include "core/input/inputcontext.h"
#include "util/log.h"
#include "util/assert.h"



InputContext::~InputContext() {
	unlinkHandler();
}



void InputContext::addState(u32 stateID, const State& state) {

	if (inputStates.contains(stateID)) {
		Log::warn("Input Context", "State with ID %d does already exist", stateID);
		return;
	}

	inputStates[stateID] = state;

	if (currentState == invalidState) {
		switchState(stateID);
	}

}



void InputContext::removeState(u32 stateID) {

	if (!inputStates.contains(stateID)) {
		Log::warn("Input Context", "State with ID %d doesn't exist", stateID);
		return;
	}

	inputStates.erase(stateID);

}



void InputContext::switchState(u32 stateID) {

	arc_assert(inputStates.contains(stateID), "State with ID %d doesn't exist", stateID);

	currentState = stateID;

}



void InputContext::addTrigger(u32 stateID, const KeyTrigger& trigger, KeyAction action) {

	arc_assert(inputStates.contains(stateID), "State with ID %d doesn't exist", stateID);

	auto& keyMap = inputStates[stateID].keyMapping;

	for (u32 i = 0; i < trigger.getKeyCount(); i++) {

		auto keyNode = keyMap.equal_range(trigger.getKey(i));

		for (auto it = keyNode.first; it != keyNode.second; it++) {

			if (it->second.first.getKeyCount() <= trigger.getKeyCount()) {
				keyMap.emplace_hint(it, trigger.getKey(i), std::make_pair(trigger, action));
				return;
			}

		}

		keyMap.emplace_hint(keyNode.second, trigger.getKey(i), std::make_pair(trigger, action));

	}

}



void InputContext::removeTrigger(u32 stateID, const KeyTrigger& trigger) {

	arc_assert(inputStates.contains(stateID), "State with ID %d doesn't exist", stateID);

	for (u32 i = 0; i < trigger.getKeyCount(); i++) {

		auto r = inputStates[stateID].keyMapping.equal_range(trigger.getKey(i));

		for (auto i = r.first; i != r.second; ++i) {
		
			if (i->second.first == trigger) {
				i = inputStates[stateID].keyMapping.erase(i);
				break;
			}
			
		}

	}

}



void InputContext::clearTriggers(u32 stateID) {

	arc_assert(inputStates.contains(stateID), "State with ID %d doesn't exist", stateID);

	inputStates[stateID].keyMapping.clear();

}



void InputContext::disable() {
	enabled = false;
}



void InputContext::enable() {
	enabled = true;
}



bool InputContext::onKeyEvent(const KeyEvent& event, const std::vector<KeyState>& keyStates) {

	if (!handler || !handler->actionListener) {
		return propagationDisabled();
	}

	Key key = event.getKey();
	auto& keyMap = inputStates[currentState].keyMapping;
	auto keyNode = keyMap.equal_range(key);
	
	for (auto it = keyNode.first; it != keyNode.second; it++) {

		if(triggerCompare(keyStates, it->second.first, event)){
			return handler->actionListener(it->second.second) || propagationDisabled();
		}

	}

	return propagationDisabled();

}



bool InputContext::onCharEvent(const CharEvent& event) {

	if (!handler || !handler->charListener || !charEnabled()) {
		return propagationDisabled();
	}

	return handler->charListener(event.getChar()) || propagationDisabled();

}



bool InputContext::onCursorEvent(const CursorEvent& event) {

	if (!handler || !handler->cursorListener || !cursorEnabled()) {
		return propagationDisabled();
	}

	return handler->cursorListener(event.getX(), event.getY()) || propagationDisabled();

}



bool InputContext::onScrollEvent(const ScrollEvent& event) {

	if (!handler || !handler->scrollListener || !scrollEnabled()) {
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



bool InputContext::charEnabled() const {
	return inputStates.at(currentState).charMode;
}



bool InputContext::cursorEnabled() const {
	return inputStates.at(currentState).enableCursor;
}



bool InputContext::scrollEnabled() const {
	return inputStates.at(currentState).enableScroll;
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