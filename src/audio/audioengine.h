#include "fmod_common.h"
#include "util/vector.h"

class Uri;

namespace FMOD {
    class Channel;
    class Reverb3D;
    class Sound;
    class System;
}


enum class SoundFormat {
    None,
    Pcm8,
    Pcm16,
    Pcm24,
    Pcm32,
    PcmFloat,
    Bitstream,
};

class AudioEngine {
public:
    AudioEngine();

    bool initialize();
    bool update();
    bool shutdown();

    FMOD::Sound* createSound(const Uri& path);
    FMOD::Channel* playSound(FMOD::Sound* sound);
    void playSound(const Uri& path);

private:
    FMOD::System* system;
};