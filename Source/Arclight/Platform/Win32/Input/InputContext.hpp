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

#include <map>
#include <unordered_map>
#include <optional>



class KeyContext;

class InputContext {

	struct Layer {

		std::unordered_multimap<Key, KeyAction> physicalKeyLookup;
		std::unordered_multimap<Key, KeyAction> virtualKeyLookup;
		std::vector<KeyAction> steadyActions;

	};

public:

	InputContext();

	void addLayer(u32 layer);
	void removeLayer(u32 layer);
	bool hasLayer(u32 layer) const;

	void addAction(KeyAction action, const KeyTrigger& trigger, bool steady = false);
	void addBoundAction(KeyAction action, const KeyTrigger& boundTrigger, const KeyTrigger& defaultTrigger, bool steady = false);
	void removeAction(KeyAction action);
	void clearActions();
	bool hasAction(KeyAction action) const;

	void addLayerAction(u32 layer, KeyAction action, const KeyTrigger& trigger, bool steady = false);
	void addBoundLayerAction(u32 layer, KeyAction action, const KeyTrigger& boundTrigger, const KeyTrigger& defaultTrigger, bool steady = false);

	void setBinding(KeyAction action, const KeyTrigger& binding);
	void restoreBinding(KeyAction action);
	void restoreAllBindings();
	const KeyTrigger& getActionBinding(KeyAction action) const;
	const KeyTrigger& getActionDefaultBinding(KeyAction action) const;
	bool isSteadyAction(KeyAction action) const;

	void registerAction(u32 layer, KeyAction action);
	void unregisterAction(u32 layer, KeyAction action);
	void unregisterActionGroup(KeyAction action);
	void unregisterLayerActions(u32 layer);
	void unregisterAllActions();
	bool actionRegistered(u32 layer, KeyAction action) const;

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

	static bool isTriggered(const KeyContext& context, const KeyTrigger& trigger, const KeyEvent& event);

	bool modal;
	bool enabled;
	InputHandler handler;

	std::map<u32, Layer> layers;
	std::unordered_map<KeyAction, std::pair<KeyTrigger, bool>> actionBindings;
	std::unordered_map<KeyAction, KeyTrigger> defaultBindings;

};