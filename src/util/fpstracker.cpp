#include "fpstracker.h"



double FPSTracker::getFPS() {

	double dt = getElapsedTime(Time::Unit::Seconds);
	frames++;

	if (dt > 0.25 && frames > 10) {

		fps = frames / dt;
		frames = 0;
		start();

	}

	return fps;

}