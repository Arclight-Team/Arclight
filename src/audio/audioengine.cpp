#include "audio/audioengine.h"
#include "util/file.h"
#include "util/log.h"

#include <fmod.hpp>
#include <fmod_errors.h>

AudioEngine::AudioEngine() : system(nullptr) {}

bool AudioEngine::initialize() {
    FMOD_RESULT result;
    result = FMOD::System_Create(&system);
    if (result != FMOD_OK)
    {
        Log::error("Audio Engine", "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        return false;
    }

    result = system->init(512, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK)
    {
        Log::error("Audio Engine", "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        return false;
    }
    return true;
}

FMOD::Sound* AudioEngine::createSound(const Uri& path) {
    FMOD::Sound* sound = nullptr;
    FMOD_RESULT result;
    result = system->createSound(path.getPath().c_str(), FMOD_DEFAULT, FMOD_DEFAULT, &sound);

    if (result != FMOD_OK) {
        Log::error("Audio Engine", "Failed to create sound: %s", FMOD_ErrorString(result));
        return nullptr;
    }

    return sound;
}

FMOD::Channel* AudioEngine::playSound(FMOD::Sound* sound) {
    FMOD::Channel* channel = nullptr;
    FMOD_RESULT result;
    result = system->playSound(sound, nullptr, false, &channel);

    if (result != FMOD_OK) {
        Log::error("Audio Engine", "Failed to play sound: %s", FMOD_ErrorString(result));
        return nullptr;
    }

    return channel;
}

void AudioEngine::playSound(const Uri& path) {
    FMOD::Sound* sound = nullptr;
    FMOD_RESULT result;
    result = system->createSound(path.getPath().c_str(), FMOD_DEFAULT, FMOD_DEFAULT, &sound);

    if (result != FMOD_OK) {
        Log::error("Audio Engine", "Failed to create sound: %s", FMOD_ErrorString(result));
        return;
    }

    result = system->playSound(sound, nullptr, false, nullptr);

    if (result != FMOD_OK) {
        Log::error("Audio Engine", "Failed to play sound: %s", FMOD_ErrorString(result));
    }
}

bool AudioEngine::update() {
    FMOD_RESULT result;
    result = system->update();

    if (result != FMOD_OK) {
        Log::error("Audio Engine", "Failed to update: %s", FMOD_ErrorString(result));
        return false;
    }

    return true;
}

bool AudioEngine::shutdown() {
    FMOD_RESULT result;
    result = system->release();

    if (result != FMOD_OK) {
        Log::error("Audio Engine", "Failed to release system: %s", FMOD_ErrorString(result));
        return false;
    }

    return true;
}
