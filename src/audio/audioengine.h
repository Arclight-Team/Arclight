#pragma once


class Uri;

namespace FMOD {
	class System;
}

class AudioEngine {

public:

	AudioEngine();

	bool initialize();
	void update();
	void shutdown();

	void playSound(const Uri& soundAsset);

private:
	FMOD::System* system;

};