/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 InputContext.cpp
 */

#include "InputContext.hpp"
#include "Util/Log.hpp"
#include "Common/Assert.hpp"
#include "KeyContext.hpp"

#include <algorithm>
#include <ranges>



InputContext::InputContext() : modal(false), enabled(true) {
	addLayer(0);
}



void InputContext::addLayer(u32 layer) {

	if (hasLayer(layer)) {
		LogW("Input Context").print("Layer with ID %d does already exist", layer);
		return;
	}

	layers.try_emplace(layer);

}



void InputContext::removeLayer(u32 layer) {

	if (!hasLayer(layer)) {
		LogW("Input Context").print("Layer with ID %d doesn't exist", layer);
		return;
	}

	layers.erase(layer);

}



bool InputContext::hasLayer(u32 layer) const {
	return layers.contains(layer);
}



void InputContext::addAction(KeyAction action, const KeyTrigger& trigger, bool steady) {

	arc_assert(!hasAction(action), "Action %d already added to context", action);

	actionBindings[action] = std::make_pair(trigger, steady);
	defaultBindings[action] = trigger;

}



void InputContext::addBoundAction(KeyAction action, const KeyTrigger& boundTrigger, const KeyTrigger& defaultTrigger, bool steady) {

	arc_assert(!hasAction(action), "Action %d already added to context", action);

	actionBindings[action] = std::make_pair(boundTrigger, steady);
	defaultBindings[action] = defaultTrigger;

}



void InputContext::removeAction(KeyAction action) {

	arc_assert(hasAction(action), "Action %d not added to context", action);

	unregisterLayerActions(action);
	actionBindings.erase(action);
	defaultBindings.erase(action);

}



void InputContext::clearActions() {

	unregisterAllActions();
	actionBindings.clear();
	defaultBindings.clear();

}



bool InputContext::hasAction(KeyAction action) const {
	return actionBindings.contains(action);
}



void InputContext::addLayerAction(u32 layer, KeyAction action, const KeyTrigger& trigger, bool steady) {

	addAction(action, trigger, steady);
	registerAction(layer, action);

}



void InputContext::addBoundLayerAction(u32 layer, KeyAction action, const KeyTrigger& boundTrigger, const KeyTrigger& defaultTrigger, bool steady) {

	addBoundAction(action, boundTrigger, defaultTrigger, steady);
	registerAction(layer, action);

}



void InputContext::setBinding(KeyAction action, const KeyTrigger& binding) {

	if (!hasAction(action)) {
		LogW("Input Context") << "Cannot set action binding without a default action trigger";
		return;
	}

	std::vector<u32> boundLayers;

	for (u32 layerID : layers | std::views::keys) {

		if (actionRegistered(layerID, action)) {

			unregisterAction(layerID, action);
			boundLayers.push_back(layerID);

		}

	}

	actionBindings[action].first = binding;

	for (u32 layer : boundLayers) {
		registerAction(layer, action);
	}

}



void InputContext::restoreBinding(KeyAction action) {

	if (!hasAction(action)) {
		LogW("Input Context") << "Cannot restore action binding without a default action trigger";
		return;
	}

	setBinding(action, defaultBindings[action]);

}



void InputContext::restoreAllBindings() {

	for (const auto & action: defaultBindings | std::views::keys) {
		restoreBinding(action);
	}

}



const KeyTrigger& InputContext::getActionBinding(KeyAction action) const {

	arc_assert(hasAction(action), "Action %d not added to context", action);

	return actionBindings.at(action).first;

}



const KeyTrigger& InputContext::getActionDefaultBinding(KeyAction action) const {

	arc_assert(hasAction(action), "Action %d not added to context", action);

	return defaultBindings.at(action);

}



bool InputContext::isSteadyAction(KeyAction action) const {

	arc_assert(hasAction(action), "Action bindings don't contain action %d", action);

	return actionBindings.at(action).second;

}



void InputContext::registerAction(u32 layer, KeyAction action) {

	arc_assert(hasLayer(layer), "Layer %d not defined for input context while registering key action %d", layer, action);
	arc_assert(hasAction(action), "Action bindings don't contain action %d", action);

	const KeyTrigger& trigger = getActionBinding(action);

	if (isSteadyAction(action)) {

		auto& steadyActions = layers[layer].steadyActions;

		for (auto it = steadyActions.begin(); it != steadyActions.end(); ++it) {

			if (getActionBinding(*it).getKeyCount() <= trigger.getKeyCount()) {
				steadyActions.insert(it, action);
				return;
			}

		}

		steadyActions.insert(steadyActions.end(), action);

	} else {

		auto& keyMap = trigger.isPhysical() ? layers[layer].physicalKeyLookup : layers[layer].virtualKeyLookup;

		for (u32 i = 0; i < trigger.getKeyCount(); i++) {

			const auto& actions = keyMap.equal_range(trigger.getKey(i));
			keyMap.emplace_hint(actions.second, trigger.getKey(i), action);

		}

	}

}



void InputContext::unregisterAction(u32 layer, KeyAction action) {

	arc_assert(hasLayer(layer), "Layer %d not defined for input context while unregistering key action %d", layer, action);
	arc_assert(hasAction(action), "Action bindings don't contain action %d", action);

	if (isSteadyAction(action)) {

		auto& coActions = layers[layer].steadyActions;

		for (u32 i = 0; i < coActions.size(); i++) {

			if (coActions[i] == action) {
				coActions.erase(coActions.begin() + i);
				return;
			}

		}

	} else {

		const KeyTrigger& trigger = getActionBinding(action);
		auto& keyMap = trigger.isPhysical() ? layers[layer].physicalKeyLookup : layers[layer].virtualKeyLookup;

		for (u32 i = 0; i < trigger.getKeyCount(); i++) {

			const auto& actions = keyMap.equal_range(trigger.getKey(i));

			for (auto it = actions.first; it != actions.second; ++it) {

				if (it->second == action) {
					keyMap.erase(it);
					break;
				}

			}

		}

	}

}



void InputContext::unregisterActionGroup(KeyAction action) {

	for (u32 key : layers | std::views::keys) {
		unregisterAction(key, action);
	}

}



void InputContext::unregisterLayerActions(u32 layer) {

	arc_assert(hasLayer(layer), "Layer %d not defined for input context while unregistering all actions", layer);

	auto& [physicalKeyLookup, virtualKeyLookup, steadyActions] = layers[layer];
	physicalKeyLookup.clear();
	virtualKeyLookup.clear();
	steadyActions.clear();

}



void InputContext::unregisterAllActions() {

	for (u32 layer : layers | std::views::keys) {
		unregisterLayerActions(layer);
	}

}



bool InputContext::actionRegistered(u32 layer, KeyAction action) const {

	arc_assert(hasLayer(layer), "Layer %d not defined for input context while unregistering all action", layer);
	arc_assert(hasAction(action), "Action bindings don't contain action %d", action);

	if (isSteadyAction(action)) {

		for (KeyAction k : layers.at(layer).steadyActions) {

			if (k == action) {
				return true;
			}

		}

	} else {

		const KeyTrigger& trigger = getActionBinding(action);
		const auto& keyMap = trigger.isPhysical() ? layers.at(layer).physicalKeyLookup : layers.at(layer).virtualKeyLookup;
		const auto& actions = keyMap.equal_range(trigger.getKey(0));

		for (auto it = actions.first; it != actions.second; ++it) {

			if (it->second == action) {
				return true;
			}

		}

	}

	return false;

}



void InputContext::disable() {
	enabled = false;
}



void InputContext::enable() {
	enabled = true;
}



bool InputContext::onKeyEvent(const KeyEvent& event, const KeyContext& keyContext) {

	if (!enabled) {
		return modal;
	}

	bool consumed = false;

	if (handler.onActionEvent) {

		auto actionInvoker = [&](const auto& actions) -> bool {

			for (auto it = actions.first; it != actions.second; ++it) {

				if (isTriggered(keyContext, getActionBinding(it->second), event)){

					if (handler.onActionEvent(ActionEvent(it->second, 1))) {

						consumed = true;
						return true;

					}

				}

			}

			return false;

		};

		for (const auto& layer : layers | std::views::values) {

			const auto& virtualActions = layer.virtualKeyLookup.equal_range(event.virtualKey);
			const auto& physicalActions = layer.physicalKeyLookup.equal_range(event.physicalKey);

			actionInvoker(virtualActions) || actionInvoker(physicalActions);

			if (consumed) {
				break;
			}

		}

	}

	if (handler.onKeyEvent) {
		consumed |= handler.onKeyEvent(event);
	}

	return consumed || modal;

}



bool InputContext::onCharEvent(const CharEvent& event) {

	if (!enabled || !handler.onCharEvent) {
		return modal;
	}

	return handler.onCharEvent(event) || modal;

}



bool InputContext::onCursorEvent(const CursorEvent& event) {

	if (!enabled || !handler.onCursorEvent) {
		return modal;
	}

	return handler.onCursorEvent(event) || modal;

}



bool InputContext::onScrollEvent(const ScrollEvent& event) {

	if (!enabled || !handler.onScrollEvent) {
		return modal;
	}

	return handler.onScrollEvent(event) || modal;

}



bool InputContext::onCursorAreaEvent(const CursorAreaEvent& event) {

	if (!enabled || !handler.onCursorAreaEvent) {
		return modal;
	}

	return handler.onCursorAreaEvent(event) || modal;

}




bool InputContext::onSteadyEvent(u32 ticks, KeyContext& keyContext) {

	if (enabled && handler.onActionEvent) {

		for (const Layer& layer : layers | std::views::values) {

			// For every steady action
			for (KeyAction action : layer.steadyActions) {

				u32 combinedEventCount = -1;
				const KeyTrigger& trigger = getActionBinding(action);
				const auto& keyMap = keyContext.getKeyMap(trigger.isPhysical());
				auto& events = keyContext.getEvents(trigger.isPhysical());

				// Traverse all keys of a given trigger
				for (u32 i = 0; i < trigger.getKeyCount(); i++) {

					Key key = trigger.getKey(i);
					u32 eventCount = ticks;

					KeyState keyState = keyMap[key] ? KeyState::Pressed : KeyState::Released;
					u32 keyEvents = events[key];

					// If the state hasn't changed during the update interval, the trigger would have been active for every tick.
					// Else, every second state flip (assuming perfectly equidistributed event ticks) adds an event.
					// If the final state happens to activate the trigger, the event count needs to be rounded up.
					if (keyState != trigger.getKeyState() || keyEvents != 0) {
						eventCount = (keyEvents + (keyState == trigger.getKeyState())) / 2;
					}

					// Given all previous key event counts, take the lowest of all for best estimation
					combinedEventCount = std::min(combinedEventCount, eventCount);

				}

				if (combinedEventCount) {

					if (handler.onSteadyEvent(ActionEvent(action, combinedEventCount))) {

						for (u32 i = 0; i < trigger.getKeyCount(); i++) {
							keyContext.clearEvent(trigger.getKey(i), trigger.isPhysical());
						}

					}

				}

			}

		}

	}

	return modal;

}



InputHandler& InputContext::getHandler() {
	return handler;
}



const InputHandler& InputContext::getHandler() const {
	return handler;
}



bool InputContext::isModal() const {
	return modal;
}



void InputContext::setModality(bool modal) {
	this->modal = modal;
}



bool InputContext::isTriggered(const KeyContext& context, const KeyTrigger& trigger, const KeyEvent& event) {

	if (trigger.getKeyState() != event.state) {
		return false;
	}

	const auto& keyMap = context.getKeyMap(trigger.isPhysical());

	for (u32 i = 0; i < trigger.getKeyCount(); i++) {

		Key key = trigger.getKey(i);
		KeyState state = keyMap[key] ? KeyState::Pressed : KeyState::Released;

		if (state != trigger.getKeyState()) {
			return false;
		}

	}

	return true;

}