#include "core/input/inputcontext.h"
#include "util/log.h"
#include "util/assert.h"



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



void InputContext::addTrigger(u32 stateID, const InputTrigger& trigger, InputAction action) {

	arc_assert(inputStates.contains(stateID), "State with ID %d doesn't exist", stateID);

	for (u32 i = 0; i < trigger.getKeyCount(); i++) {
		inputStates[stateID].keyMapping.emplace(trigger.getKey(i), std::make_pair(trigger, action));
	}

}



void InputContext::removeTrigger(u32 stateID, const InputTrigger& trigger) {

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



bool InputContext::onKeyEvent(const KeyEvent& event) {

	if (!handler) {
		return false;
	}



}



bool InputContext::onCharEvent(const CharEvent& event) {

	if (!handler || !inputStates[currentState].enableChar) {
		return false;
	}

	return handler->onCharReceived(event.getChar());

}



bool InputContext::onCursorEvent(const CursorEvent& event) {

	if (!handler || !inputStates[currentState].enableCursorMove) {
		return false;
	}

	return handler->onCursorMove(event);

}



bool InputContext::onScrollEvent(const ScrollEvent& event) {

	if (!handler || !inputStates[currentState].enableScroll) {
		return false;
	}

	return handler->onScroll(event);

}



void InputContext::linkHandler(InputHandler& handler) {

	this->handler = &handler;

}



void InputContext::unlinkHandler() {

}



u32 InputContext::getCurrentStateID() const {
	return currentState;
}



u32 InputContext::getPriority() const {
	return priority;
}



const std::string& InputContext::getContextName() const {
	return name;
}