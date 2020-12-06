#include "input/inputhandler.h"
#include "input/inputcontext.h"




InputHandler::~InputHandler() {

	if (context) {
		context->unlinkHandler();
	}

}