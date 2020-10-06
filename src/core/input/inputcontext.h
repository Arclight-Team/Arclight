#pragma once

#include <unordered_map>


typedef u32 InputTrigger;
typedef u32 InputAction;


class InputContext {

public:

	typedef std::unordered_map<InputTrigger, InputAction> InputMapping;

	void switchState(u32 stateID);
	void enableTrigger(const InputTrigger& trigger, InputAction action);
	void disableTrigger(const InputTrigger& trigger);

	u32 getCurrentStateID() const;

private:
	std::string name;
	u32 priority;
	u32 currentState;
	std::unordered_map<u32, InputMapping> inputStates;

};