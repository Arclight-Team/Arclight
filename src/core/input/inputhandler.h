#pragma once

#include "core/input/keytrigger.h"
#include "core/input/inputevent.h"


class InputContext;

class InputHandler {

public:

	explicit InputHandler(InputContext* context);
	~InputHandler();

	InputHandler(const InputHandler& handler) = delete;
	InputHandler& operator=(const InputHandler& handler) = delete;

	bool onActionTriggered(KeyAction action);
	bool onCharReceived(KeyChar character);
	bool onCursorMove(const CursorEvent& event);
	bool onScroll(const ScrollEvent& event);

private:

	InputContext* context;

};