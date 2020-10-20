#pragma once

#include "core/input/keytrigger.h"
#include "core/input/inputhandler.h"
#include <unordered_map>
#include <optional>


class KeyEvent;
class CharEvent;
class CursorEvent;
class ScrollEvent;

class InputContext {

	struct State {

		inline State(bool disablePropagation = false) : disablePropagation(disablePropagation) {}

		bool disablePropagation;
		std::unordered_multimap<Key, KeyAction> keyLookup;

	};

public:

	constexpr static u32 invalidState = -1;

	inline explicit InputContext() : enabled(true), handler(nullptr), currentState(invalidState) {}
	~InputContext();

	InputContext(const InputContext& context) = delete;
	InputContext& operator=(const InputContext& context) = delete;
	InputContext(InputContext&& context) noexcept = default;
	InputContext& operator=(InputContext&& context) = default;

	void addState(u32 stateID, bool disablePropagation = false);
	void removeState(u32 stateID);
	void switchState(u32 stateID);
	bool stateAdded(u32 stateID) const;

	void addBoundAction(KeyAction action, const KeyTrigger& boundTrigger, const KeyTrigger& defaultTrigger);
	void addAction(KeyAction action, const KeyTrigger& trigger);
	void removeAction(KeyAction action);
	void clearActions();
	bool actionAdded(KeyAction action) const;

	void setBinding(KeyAction action, const KeyTrigger& binding);
	void restoreBinding(KeyAction action);
	void restoreBindings();
	KeyTrigger getActionBinding(KeyAction action) const;
	KeyTrigger getActionDefaultBinding(KeyAction action) const;

	void registerAction(u32 stateID, KeyAction action);
	void unregisterAction(u32 stateID, KeyAction action);
	void unregisterAction(KeyAction action);
	void unregisterActions(u32 stateID);
	void unregisterActions();
	bool actionRegistered(u32 stateID, KeyAction action) const;

	void disable();
	void enable();

	bool onKeyEvent(const KeyEvent& event, const std::vector<KeyState>& keyStates);
	bool onCharEvent(const CharEvent& event);
	bool onCursorEvent(const CursorEvent& event);
	bool onScrollEvent(const ScrollEvent& event);

	void linkHandler(InputHandler& handler);
	void unlinkHandler();

	u32 getCurrentStateID() const;
	bool propagationDisabled() const;

private:

	bool triggerCompare(const std::vector<KeyState>& keyStates, const KeyTrigger& trigger, const KeyEvent& event) const;

	bool enabled;
	u32 currentState;
	InputHandler* handler;
	std::unordered_map<u32, State> inputStates;
	std::unordered_map<KeyAction, KeyTrigger> actionBindings;
	std::unordered_map<KeyAction, KeyTrigger> defaultBindings;

};