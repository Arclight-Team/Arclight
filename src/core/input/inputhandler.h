#pragma once

#include "core/input/inputtrigger.h"
#include "core/input/inputevent.h"


class InputContext;

class InputHandler {

public:

	explicit InputHandler(InputContext* context);
	~InputHandler();

	bool onActionTriggered(InputAction action);
	bool onCharReceived(KeyChar character);
	bool onCursorMove(const CursorEvent& event);
	bool onScroll(const ScrollEvent& event);

private:

	InputContext* context;

};