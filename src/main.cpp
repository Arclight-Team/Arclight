#include "util/log.h"
#include "util/vector.h"
#include "util/format.h"
#include "util/assert.h"


int main(){

	Log::openLogFile();

	Log::info("Core", "Starting game engine");

	Vec2 a(1.2, 3.9);
	Vec2 b(0.3, 1.11);
	double x = Math::toDegrees(a.angle(b));

	arc_assert(1 < -32, "dd%s %f {appname}", "kkkkkek", 3);
	

	Log::debug("Core", "{appname}: The angle between the two vectors equals %f", x);
	
	Log::closeLogFile();

	return 0;

}
