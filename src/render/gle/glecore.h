#pragma once

#include "gc.h"


GLE_BEGIN

namespace Core {

	//Initializes the context and returns whether it was successful. Call once after the context has been created.
	bool init();

	void printErrors();

}


GLE_END