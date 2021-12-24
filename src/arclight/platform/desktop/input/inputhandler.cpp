#include "inputhandler.hpp"
#include "inputcontext.hpp"




InputHandler::~InputHandler() {

	if (context) {
		context->unlinkHandler();
	}

}