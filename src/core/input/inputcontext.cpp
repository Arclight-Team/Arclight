#include "core/input/inputcontext.h"
#include "util/log.h"



void InputContext::switchState(u32 stateID) {
	currentState = stateID;
}



void InputContext::addTrigger(const InputTrigger& trigger, InputAction action) {

	for (u32 i = 0; i < trigger.getKeyCount(); i++) {
		inputStates[currentState].emplace(trigger.getKey(i), std::make_pair(trigger, action));
	}

}



void InputContext::removeTrigger(const InputTrigger& trigger) {

	for (u32 i = 0; i < trigger.getKeyCount(); i++) {

		auto r = inputStates[currentState].equal_range(trigger.getKey(i));

		for (auto i = r.first; i != r.second; ++i) {
		
			if (i->second.first == trigger) {
				i = inputStates[currentState].erase(i);
				break;
			}
			
		}

	}

}



void InputContext::clearTriggers() {

	for (auto& state : inputStates) {
		state.second.clear();
	}

	inputStates.clear();

}



void InputContext::disable() {
	enabled = false;
}



void InputContext::enable() {
	enabled = true;
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