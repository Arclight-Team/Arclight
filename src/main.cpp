#include "core/engine.h"



int main() {

	Engine engine;

	if (!engine.initialize()) {
		return -1;
	}

	engine.run();
	engine.shutdown();

	return 0;

}