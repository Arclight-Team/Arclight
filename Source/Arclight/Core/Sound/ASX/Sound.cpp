/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Sound.cpp
 */

#include "Sound.hpp"


// ASX - Arclight FMOD wrapper
namespace ASX
{
	
	void Sound::create(const Path& path, const SoundOptions& opt) {

		// Already constructed
		if (handle) {
			return;
		}

		if (opt.nonBlocking)
			nonBlocking = true;

		handle = nullptr;
		ASX_TRY() = getSystemHandle()->createSound(path.toString().c_str(), opt.getFlag(), nullptr, &handle);

	}

	void Sound::create(const Path& path, CreateSoundInfo& info, const SoundOptions& opt) {

		// Already constructed
		if (handle) {
			return;
		}

		if (opt.nonBlocking)
			nonBlocking = true;

		handle = nullptr;
		ASX_TRY() = getSystemHandle()->createSound(path.toString().c_str(), opt.getFlag(), &info, &handle);

	}

	void Sound::destroy() {

		if (handle) {
			ASX_TRY() = handle->release();
			handle = nullptr;
		}

	}

	void Sound::play(Channel& channel, bool paused) {

		FMOD::Channel* fmodHandle{};

		playImpl(paused, nullptr, &fmodHandle);

		// Acquire new handle and release old one
		channel.acquire(fmodHandle);

	}

	void Sound::play(Channel& channel, const SoundOptions& opt, bool paused) {

		FMOD::Channel* fmodHandle{};

		playImpl(opt, paused, nullptr, &fmodHandle);

		// Acquire new handle and release old one
		channel.acquire(fmodHandle);

	}

	void Sound::play(ChannelGroup& group, Channel& channel, bool paused) {

		FMOD::Channel* fmodHandle{};

		playImpl(paused, group.handle, &fmodHandle);

		// Acquire new handle and release old one
		channel.acquire(fmodHandle);

	}

	void Sound::play(ChannelGroup& group, Channel& channel, const SoundOptions& opt, bool paused) {

		FMOD::Channel* fmodHandle{};

		playImpl(opt, paused, group.handle, &fmodHandle);

		// Acquire new handle and release old one
		channel.acquire(fmodHandle);

	}

	void Sound::setDefaults(const SoundDefaults& defaults) {
		ASX_TRY() = handle->setDefaults(defaults.frequency, defaults.priority);
	}

	SoundDefaults Sound::getDefaults() const {

		float frequency{};
		i32 priority{};

		ASX_TRY() = handle->getDefaults(&frequency, &priority);

		return { frequency, priority };

	}

	void Sound::set3DMinMaxDistance(const Sound3DMinMaxDistance& distance) {
		ASX_TRY() = handle->set3DMinMaxDistance(distance.min, distance.max);
	}

	Sound3DMinMaxDistance Sound::get3DMinMaxDistance() const {

		float min;
		float max;

		ASX_TRY() = handle->get3DMinMaxDistance(&min, &max);

		return { min, max };

	}

	void Sound::set3DConeSettings(const ConeSettings& settings) {
		ASX_TRY() = handle->set3DConeSettings(settings.insideConeAngle, settings.outsideConeAngle, settings.outsideVolume);
	}

	ConeSettings Sound::get3DConeSettings() const {

		ConeSettings settings;

		ASX_TRY() = handle->get3DConeSettings(&settings.insideConeAngle, &settings.outsideConeAngle, &settings.outsideVolume);

		return settings;

	}

	void Sound::set3DCustomRolloff(std::span<Vec3f> points) {

		auto list = castVec(points.data());
		auto size = points.size();

		ASX_TRY() = handle->set3DCustomRolloff(list, size);

	}

	std::span<const Vec3f> Sound::get3DCustomRolloff() const {

		FMOD_VECTOR* points;
		int numpoints;

		ASX_TRY() = handle->get3DCustomRolloff(&points, &numpoints);

		return { castVec(points), SizeT(numpoints) };

	}
	
	void Sound::getSubSound(int index, Sound& sound) {

		FMOD::Sound* fmodSound{};

		ASX_TRY() = handle->getSubSound(index, &fmodSound);

		sound.handle = fmodSound;

	}

	void Sound::getSubSoundParent(Sound& parent) {

		FMOD::Sound* fmodSound{};

		ASX_TRY() = handle->getSubSoundParent(&fmodSound);

		parent.handle = fmodSound;

	}

	std::string Sound::getName() const {

		std::string name;
		name.resize(1024);

		ASX_TRY() = handle->getName(name.data(), name.size());

		name.shrink_to_fit();

		return name;

	}

	u32 Sound::getLength(TimeUnit unit) const {

		u32 length{};

		ASX_TRY() = handle->getLength(&length, Conv::fromTimeUnit(unit));

		return length;

	}

	SoundFormatInfo Sound::getFormatInfo() const {

		FMOD_SOUND_TYPE type;
		FMOD_SOUND_FORMAT format;
		i32 channels, bits;

		ASX_TRY() = handle->getFormat(&type, &format, &channels, &bits);

		return { SoundType(type), SoundFormat(format), u32(channels), u32(bits) };

	}

	u32 Sound::getSubSoundCount() const {

		int count{};

		ASX_TRY() = handle->getNumSubSounds(&count);

		return count;

	}

	// getTagCount

	std::optional<FMOD_TAG> Sound::getTag(const std::string& name, int index) const {

		FMOD_TAG tag{};
		const char* tagName = name.empty() ? nullptr : name.c_str();
		int tagIndex = index;

		// TODO: cheat
		return handle->getTag(tagName, tagIndex, &tag) != FMOD_OK ? std::nullopt : std::optional(tag);

	}
	
	SoundOpenStateInfo Sound::getOpenState() const {

		FMOD_OPENSTATE openState;
		SoundOpenStateInfo info{};

		ASX_TRY() = handle->getOpenState(&openState, &info.percentBuffered, &info.starving, &info.diskBusy);

		info.openState = SoundOpenState(openState);

		return info;

	}
	
	void Sound::seekData(u32 offsetPCM) {
		ASX_TRY() = handle->seekData(offsetPCM);
	}

	// setSoundGroup
	// getSoundGroup

	u32 Sound::getSyncPointCount() const {

		int count{};

		ASX_TRY() = handle->getNumSyncPoints(&count);

		return count;

	}

	// getSyncPoint
	// getSyncPointInfo
	// addSyncPoint
	// deleteSyncPoint

	// setMode
	// getMode

	void Sound::setLoopCount(u32 count) {
		ASX_TRY() = handle->setLoopCount(count);
	}

	u32 Sound::getLoopCount() const {

		int count{};

		ASX_TRY() = handle->getLoopCount(&count);

		return count;

	}

	void Sound::setLoopPoints(const TimePoint& start, const TimePoint& end) {
		ASX_TRY() = handle->setLoopPoints(start.value, start.getUnit(), end.value, end.getUnit());
	}

	LoopPoints Sound::getLoopPoints(TimeUnit startUnit, TimeUnit endUnit) {

		u32 start{}, end{};

		ASX_TRY() = handle->getLoopPoints(&start, Conv::fromTimeUnit(startUnit), &end, Conv::fromTimeUnit(endUnit));

		return { start, end };

	}

	// getMusicChannelCount
	// setMusicChannelVolume
	// getMusicChannelVolume
	// setMusicSpeed
	// getMusicSpeed

	void Sound::playImpl(bool paused, FMOD::ChannelGroup* group, FMOD::Channel** channel) {

		if (!handle) {
			return;
		}

		if (nonBlocking) 
			ASX_TRY(FMOD_ERR_NOTREADY) = getSystemHandle()->playSound(handle, group, paused, channel);
		else 
			ASX_TRY() = getSystemHandle()->playSound(handle, group, paused, channel);

	}

	void Sound::playImpl(const SoundOptions& opt, bool paused, FMOD::ChannelGroup* group, FMOD::Channel** channel) {

		if (!handle) {
			return;
		}

		u32 mode = opt.getFlag();
		if (mode) {
			handle->setMode(mode);
		}

		if (opt.nonBlocking)
			nonBlocking = true;

		if (nonBlocking)
			ASX_TRY(FMOD_ERR_NOTREADY) = getSystemHandle()->playSound(handle, group, paused, channel);
		else
			ASX_TRY() = getSystemHandle()->playSound(handle, group, paused, channel);

	}

}
