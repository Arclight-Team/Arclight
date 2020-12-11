#include "fpstracker.h"



double FPSTracker::getFPS() {

	double dt = getElapsedTime(Timer::Unit::Nanoseconds) / 1000000000.0;
	frames++;

	if (dt > 0.25 && frames > 10) {

		fps = frames / dt;
		frames = 0;
		start();

	}

	return fps;

}