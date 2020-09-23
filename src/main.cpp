#include "util/log.h"
#include "util/vector.h"
#include <iostream>


int main(){

	Log::openLogFile();

	Log::info("Core", "Starting game engine");
	Log::info("Core", "Starting game engine");

	Vec2 a(1.2, 3.9);
	Vec2 b(0.3, 1.11);
	double x = Math::toDegrees(a.angle(b));
	std::cout << x << std::endl;
	

	Log::closeLogFile();

	return 0;

}
