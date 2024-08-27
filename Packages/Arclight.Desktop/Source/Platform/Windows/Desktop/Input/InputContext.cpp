/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 InputContext.cpp
 */

#include "Desktop/Input/InputContext.hpp"
#include "Desktop/Input/KeyContext.hpp"
#include "Common/Assert.hpp"
#include "Util/Log.hpp"

#include <algorithm>
#include <ranges>



InputContext::InputContext() : modal(false), enabled(true) {
	setLayerCount(1);
}



void InputContext::setLayerCount(u32 count) {

	if (count == 0) {

		LogW("Input Context") << "Layer count cannot be 0";
		return;

	}

	if (layers.size() > count) {

		for (SizeT i = count; i < layers.size(); i++) {

			for (const auto& [action, _1, _2] : layers[i].actions) {
				removeAction(action);
			}

			for (const auto& [action, _1, _2] : layers[i].steadyActions) {
				removeAction(action);
			}

		}

	}

	layers.resize(count);

}



u32 InputContext::layerCount() const {
	return layers.size();
}



void InputContext::disableLayer(u32 layerID) {
	getLayer(layerID).enabled = false;
}



void InputContext::enableLayer(u32 layerID) {
	getLayer(layerID).enabled = false;
}



bool InputContext::isLayerEnabled(u32 layerID) const {
	return getLayer(layerID).enabled;
}



void InputContext::addAction(u32 layerID, KeyAction action, const KeyTrigger& trigger, bool steady) {
	addBoundAction(layerID, action, trigger, trigger, steady);
}



void InputContext::addBoundAction(u32 layerID, KeyAction action, const KeyTrigger& boundTrigger, const KeyTrigger& defaultTrigger, bool steady) {

	if (hasAction(action)) {

		LogW("Input Context") << "Action " << action << " already exists";
		return;

	}

	Layer& layer = getLayer(layerID);
	(steady ? layer.steadyActions : layer.actions).emplace_back(action, boundTrigger, defaultTrigger);

	actionInfo.emplace(action, ActionInfo(layerID, steady));

}



void InputContext::removeAction(KeyAction action) {

	if (actionInfo.contains(action)) {

		Layer& layer = getLayer(actionInfo[action].layerID);

		auto it = layer.actions.erase(std::ranges::find_if(layer.actions, [action](const ActionBinding& binding) {
			return binding.action == action;
		}));

		if (it == layer.actions.end()) {

			layer.steadyActions.erase(std::ranges::find_if(layer.steadyActions, [action](const ActionBinding& binding) {
				return binding.action == action;
			}));

		}

		actionInfo.erase(action);

	}

}



void InputContext::clearActions() {

	for (Layer& layer : layers) {

		layer.actions.clear();
		layer.steadyActions.clear();

	}

	actionInfo.clear();

}



bool InputContext::hasAction(KeyAction action) const {
	return actionInfo.contains(action);
}



void InputContext::setBinding(KeyAction action, const KeyTrigger& binding) {

	if (!hasAction(action)) {

		LogW("Input Context") << "Action " << action << " does not exist";
		return;

	}

	getActionBinding(action).trigger = binding;

}



void InputContext::restoreBinding(KeyAction action) {

	if (!hasAction(action)) {

		LogW("Input Context") << "Action " << action << " does not exist";
		return;

	}

	getActionBinding(action).restore();

}



void InputContext::restoreAllBindings() {

	for (Layer& layer : layers) {

		for (ActionBinding& binding : layer.actions) {
			binding.restore();
		}

		for (ActionBinding& binding : layer.steadyActions) {
			binding.restore();
		}

	}

}



const KeyTrigger& InputContext::getActionTrigger(KeyAction action) const {

	arc_assert(hasAction(action), "Action %d not added to context", action);

	return getActionBinding(action).trigger;

}



const KeyTrigger& InputContext::getActionDefaultTrigger(KeyAction action) const {

	arc_assert(hasAction(action), "Action %d not added to context", action);

	return getActionBinding(action).defaultTrigger;

}



bool InputContext::isSteadyAction(KeyAction action) const {

	arc_assert(hasAction(action), "Action bindings don't contain action %d", action);

	return actionInfo.at(action).steady;

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

		for (const Layer& layer : layers) {

			if (!layer.enabled) {
				continue;
			}

			for (const ActionBinding& binding : layer.actions) {

				if (isTriggered(keyContext, binding.trigger, event) && handler.onActionEvent(ActionEvent(binding.action, 1))) {

					consumed = true;
					break;

				}

			}

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

		for (const Layer& layer : layers) {

			if (!layer.enabled) {
				continue;
			}

			// For every steady action
			for (const ActionBinding& binding : layer.steadyActions) {

				KeyAction action = binding.action;
				const KeyTrigger& trigger = getActionTrigger(action);
				u32 modifiers = trigger.getModifiers();

				const auto& keyMap = keyContext.getKeyMap(trigger.isPhysical());
				auto& events = keyContext.getEvents(trigger.isPhysical());

				u32 combinedEventCount = -1;

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

				// Modifiers also have to be taken into account
				if (modifiers) {

					constexpr static u32 modMasks[4] = { KeyModifier::Shift, KeyModifier::Control, KeyModifier::Super, KeyModifier::Alt };

					u32 currentModifierState = keyContext.getModifierState();

					for (u32 modMask : modMasks) {

						if (modifiers & modMask) {

							u32 eventCount = ticks;

							KeyState keyState = currentModifierState & modMask ? KeyState::Pressed : KeyState::Released;
							u32 keyEvents = keyContext.getModifierEventCount(modMask);

							if (keyState != trigger.getKeyState() || keyEvents != 0) {
								eventCount = (keyEvents + (keyState == trigger.getKeyState())) / 2;
							}

							combinedEventCount = std::min(combinedEventCount, eventCount);

						}

					}

				}

				// If events lined up, trigger a steady event
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



InputContext::ActionBinding& InputContext::getActionBinding(KeyAction action) {
	return const_cast<ActionBinding&>(std::as_const(*this).getActionBinding(action));
}



const InputContext::ActionBinding& InputContext::getActionBinding(KeyAction action) const {

	const Layer& layer = getLayer(actionInfo.at(action).layerID);

	auto it = std::ranges::find_if(layer.actions, [action](const ActionBinding& binding) {
		return binding.action == action;
	});

	if (it == layer.actions.end()) {

		it = std::ranges::find_if(layer.steadyActions, [action](const ActionBinding& binding) {
			return binding.action == action;
		});

		arc_assert(it != layer.steadyActions.end(), "Action binding not found");

	}

	return *it;

}



InputContext::Layer& InputContext::getLayer(u32 layerID) {
	return layers[layerID];
}



const InputContext::Layer& InputContext::getLayer(u32 layerID) const {
	return layers[layerID];
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

	if (trigger.getModifiers() != context.getModifierState()) {
		return false;
	}

	return true;

}