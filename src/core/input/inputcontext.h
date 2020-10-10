#pragma once

#include "core/input/inputtrigger.h"
#include "core/input/inputevent.h"
#include "core/input/inputhandler.h"
#include <unordered_map>


class KeyEvent;
class CharEvent;
class CursorEvent;
class ScrollEvent;

class InputContext {

	typedef std::pair<InputTrigger, InputAction> InputMapping;

	struct State {

		inline State() : enableChar(false), enableCursorMove(true), enableScroll(true), disableCursor(false) {}
		inline State(bool enableChar, bool enableCursorMove, bool enableScroll, bool disableCursor)
			: enableChar(enableChar), enableCursorMove(enableCursorMove), enableScroll(enableScroll), disableCursor(disableCursor) {}

		bool enableChar;
		bool enableCursorMove;
		bool enableScroll;
		bool disableCursor;
		std::unordered_multimap<Key, InputMapping> keyMapping;

	};

public:

	constexpr static u32 invalidState = -1;


	inline explicit InputContext(const std::string& name, u32 priority) : enabled(true), handler(nullptr), name(name), priority(priority), currentState(invalidState) {}

	void addState(u32 stateID, const State& state = State());
	void removeState(u32 stateID);
	void switchState(u32 stateID);

	void addTrigger(u32 stateID, const InputTrigger& trigger, InputAction action);
	void removeTrigger(u32 stateID, const InputTrigger& trigger);
	void clearTriggers(u32 stateID);

	void disable();
	void enable();

	bool onKeyEvent(const KeyEvent& event);
	bool onCharEvent(const CharEvent& event);
	bool onCursorEvent(const CursorEvent& event);
	bool onScrollEvent(const ScrollEvent& event);

	void linkHandler(InputHandler& handler);
	void unlinkHandler();

	u32 getCurrentStateID() const;
	u32 getPriority() const;
	const std::string& getContextName() const;

private:

	bool enabled;
	u32 priority;
	u32 currentState;
	std::string name;
	InputHandler* handler;
	std::unordered_map<u32, State> inputStates;

};