#pragma once

#include "inputtrigger.h"
#include "inputevent.h"
#include <unordered_map>



class InputContext {

public:

	typedef std::pair<InputTrigger, InputAction> InputMapping;
	constexpr static u32 invalidState = -1;


	inline explicit InputContext(const std::string& name, u32 priority) : name(name), priority(priority), currentState(invalidState) {}

	void switchState(u32 stateID);
	void addTrigger(const InputTrigger& trigger, InputAction action);
	void removeTrigger(const InputTrigger& trigger);

	u32 getCurrentStateID() const;

private:
	std::string name;
	u32 priority;
	u32 currentState;
	std::unordered_map<u32, std::unordered_multimap<Key, InputMapping>> inputStates;

};