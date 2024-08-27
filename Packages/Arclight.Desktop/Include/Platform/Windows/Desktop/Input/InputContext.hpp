/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 InputContext.hpp
 */

#pragma once

#include "KeyTrigger.hpp"
#include "InputHandler.hpp"

#include <unordered_map>



class KeyContext;

class InputContext {

	struct ActionBinding {

		ActionBinding() : action(0) {}
		ActionBinding(KeyAction action, const KeyTrigger& trigger, const KeyTrigger& defaultTrigger)
			: action(action), trigger(trigger), defaultTrigger(defaultTrigger) {}

		void restore() {
			trigger = defaultTrigger;
		}

		auto operator<=>(const ActionBinding& binding) const {
			return trigger <=> binding.trigger;
		}

		KeyAction action;
		KeyTrigger trigger;
		KeyTrigger defaultTrigger;

	};

	struct Layer {

		constexpr Layer() : enabled(true) {}

		std::vector<ActionBinding> actions;
		std::vector<ActionBinding> steadyActions;
		bool enabled;

	};

public:

	InputContext();

	void setLayerCount(u32 count);
	u32 layerCount() const;

	void disableLayer(u32 layerID);
	void enableLayer(u32 layerID);
	bool isLayerEnabled(u32 layerID) const;

	void addAction(u32 layerID, KeyAction action, const KeyTrigger& trigger, bool steady = false);
	void addBoundAction(u32 layerID, KeyAction action, const KeyTrigger& boundTrigger, const KeyTrigger& defaultTrigger, bool steady = false);
	void removeAction(KeyAction action);
	void clearActions();
	bool hasAction(KeyAction action) const;

	void setBinding(KeyAction action, const KeyTrigger& binding);
	void restoreBinding(KeyAction action);
	void restoreAllBindings();
	const KeyTrigger& getActionTrigger(KeyAction action) const;
	const KeyTrigger& getActionDefaultTrigger(KeyAction action) const;
	bool isSteadyAction(KeyAction action) const;

	void disable();
	void enable();

	bool onKeyEvent(const KeyEvent& event, const KeyContext& keyContext);
	bool onCharEvent(const CharEvent& event);
	bool onCursorEvent(const CursorEvent& event);
	bool onScrollEvent(const ScrollEvent& event);
	bool onCursorAreaEvent(const CursorAreaEvent& event);
	bool onSteadyEvent(u32 ticks, KeyContext& keyContext);

	InputHandler& getHandler();
	const InputHandler& getHandler() const;

	bool isModal() const;
	void setModality(bool modal);

private:

	ActionBinding& getActionBinding(KeyAction action);
	const ActionBinding& getActionBinding(KeyAction action) const;

	Layer& getLayer(u32 layerID);
	const Layer& getLayer(u32 layerID) const;

	static bool isTriggered(const KeyContext& context, const KeyTrigger& trigger, const KeyEvent& event);

	bool modal;
	bool enabled;
	InputHandler handler;

	struct ActionInfo {

		constexpr ActionInfo() : layerID(0), steady(false) {}
		constexpr ActionInfo(u32 layerID, bool steady) : layerID(layerID), steady(steady) {}

		u32 layerID;
		bool steady;

	};

	std::vector<Layer> layers;
	std::unordered_map<KeyAction, ActionInfo> actionInfo;

};