#include "core/input/inputcontext.h"


void InputContext::switchState(u32 stateID) {


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



u32 InputContext::getCurrentStateID() const {
	return currentState;
}