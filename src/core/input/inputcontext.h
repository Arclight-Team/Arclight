#pragma once

#include "core/input/keytrigger.h"
#include "core/input/inputevent.h"
#include "core/input/inputhandler.h"
#include <unordered_map>


class KeyEvent;
class CharEvent;
class CursorEvent;
class ScrollEvent;

class InputContext {

public:

	typedef std::pair<KeyTrigger, KeyAction> KeyTriggers;

	struct State {

		inline State() : charMode(false), enableCursor(true), enableScroll(true), disablePropagation(false) {}
		inline State(bool charMode, bool enableCursor, bool enableScroll, bool disablePropagation)
			: charMode(charMode), enableCursor(enableCursor), enableScroll(enableScroll), disablePropagation(disablePropagation){}

		bool charMode;
		bool enableCursor;
		bool enableScroll;
		bool disablePropagation;
		std::unordered_multimap<Key, KeyTriggers> keyMapping;

	};

	constexpr static u32 invalidState = -1;

	inline explicit InputContext() : enabled(true), handler(nullptr), currentState(invalidState) {}
	~InputContext();

	InputContext(const InputContext& context) = delete;
	InputContext& operator=(const InputContext& context) = delete;
	InputContext(InputContext&& context) noexcept = default;
	InputContext& operator=(InputContext&& context) = default;

	void addState(u32 stateID, const State& state = State());
	void removeState(u32 stateID);
	void switchState(u32 stateID);

	void addTrigger(u32 stateID, const KeyTrigger& trigger, KeyAction action);
	void removeTrigger(u32 stateID, const KeyTrigger& trigger);
	void clearTriggers(u32 stateID);

	void disable();
	void enable();

	bool onKeyEvent(const KeyEvent& event, const std::vector<KeyState>& keyStates);
	bool onCharEvent(const CharEvent& event);
	bool onCursorEvent(const CursorEvent& event);
	bool onScrollEvent(const ScrollEvent& event);

	void linkHandler(InputHandler& handler);
	void unlinkHandler();

	u32 getCurrentStateID() const;
	bool charEnabled() const;
	bool cursorEnabled() const;
	bool scrollEnabled() const;
	bool propagationDisabled() const;

private:

	bool triggerCompare(const std::vector<KeyState>& keyStates, const KeyTrigger& trigger, const KeyEvent& event) const;

	bool enabled;
	u32 currentState;
	InputHandler* handler;
	std::unordered_map<u32, State> inputStates;

};