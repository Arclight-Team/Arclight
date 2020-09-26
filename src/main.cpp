#include "util/log.h"
#include "util/vector.h"
#include "util/format.h"
#include "util/assert.h"
#include "util/matrix.h"
#include "util/file.h"
#include "util/profiler.h"


int main(){

	Log::openLogFile();

	Log::info("Core", "Starting game engine");

	Profiler profiler(Profiler::Resolution::Milliseconds, 3);
	profiler.start();

	File file;
	bool open = file.open(":/textures/lol.txt", File::Out);
	arc_assert(open, "Failed to open file '%s'", file.getURI().getPath().c_str());
	file.write("KEKW");
	for (u32 i = 0; i < 100000; i++) {
		file.write(std::to_string(i));
	}
	file.close();

	profiler.stop("File 1");
	profiler.start();

	std::ofstream s(URI(":/textures/lol.txt").getPath());
	for (u32 i = 0; i < 100000; i++) {
		s << std::to_string(i);
	}
	s.close();

	profiler.stop("File 2");

	Log::closeLogFile();

	return 0;

}
