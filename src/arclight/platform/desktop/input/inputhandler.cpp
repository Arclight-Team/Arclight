/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 inputhandler.cpp
 */

#include "inputhandler.hpp"
#include "inputcontext.hpp"




InputHandler::~InputHandler() {

	if (context) {
		context->unlinkHandler();
	}

}