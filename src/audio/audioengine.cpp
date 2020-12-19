#include "audio/audioengine.h"
#include "util/file.h"
#include "util/log.h"

#include <fmod.hpp>
#include <fmod_errors.h>



AudioEngine::AudioEngine() : system(nullptr) {}



bool AudioEngine::initialize() {

    FMOD_RESULT result;

    result = FMOD::System_Create(&system);

    if (result != FMOD_OK) {
        Log::error("Audio Engine", "Failed to initialize audio backend: %s", FMOD_ErrorString(result));
        return false;
    }

    result = system->init(512, FMOD_INIT_NORMAL, 0);

    if (result != FMOD_OK) {
        Log::error("Audio Engine", "Failed to initialize audio system: %s", FMOD_ErrorString(result));
        return false;
    }

    return true;

}



void AudioEngine::update() {

    FMOD_RESULT result = system->update();

    if (result != FMOD_OK) {
        Log::error("Audio Engine", "Failed to update audio system: %s", FMOD_ErrorString(result));
    }

}


void AudioEngine::shutdown() {

    FMOD_RESULT result = system->release();

    if (result != FMOD_OK) {
        Log::error("Audio Engine", "Failed to release audio system: %s", FMOD_ErrorString(result));
    }

}



void AudioEngine::playSound(const Uri& soundAsset) {

    FMOD::Sound* sound = nullptr;
    FMOD_RESULT result = system->createSound(soundAsset.getPath().c_str(), FMOD_DEFAULT, FMOD_DEFAULT, &sound);

    if (result != FMOD_OK) {
        Log::error("Audio Engine", "Failed to create sound: %s", FMOD_ErrorString(result));
        return;
    }

    result = system->playSound(sound, nullptr, false, nullptr);

    if (result != FMOD_OK) {
        Log::error("Audio Engine", "Failed to play sound: %s", FMOD_ErrorString(result));
        return;
    }

}