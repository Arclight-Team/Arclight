#pragma once


class URI;

namespace FMOD {
	class System;
}

class AudioEngine {

public:

	AudioEngine();

	bool initialize();
	void update();
	void shutdown();

	void playSound(const URI& soundAsset);

private:
	FMOD::System* system;

};