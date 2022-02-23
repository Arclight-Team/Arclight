/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 sound.hpp
 */

#pragma once
#include "common.hpp"
#include "exception.hpp"
#include "channel.hpp"
#include "system.hpp"

#include <span>
#include <optional>



class Path;

// ASX - Arclight FMOD wrapper
namespace ASX
{

	// Structures

	struct SoundDefaults
	{
		float frequency;
		i32 priority;
	};

	struct Sound3DMinMaxDistance
	{
		float min;
		float max;
	};

	class Sound : public IHandleOwner<FMOD::Sound>, public ISystemUser
	{
	public:

		void create(const Path& path, const SoundOptions& opt = {});
		void create(const Path& path, CreateSoundInfo& info, const SoundOptions& opt = {});

		void destroy();

		void play(Channel& channel, bool paused = false);
		void play(Channel& channel, const SoundOptions& opt, bool paused = false);
		void play(ChannelGroup& group, Channel& channel, bool paused = false);
		void play(ChannelGroup& group, Channel& channel, const SoundOptions& opt, bool paused = false);

		inline void play(bool paused = false) {
			play(channel, paused);
		}

		inline void play(const SoundOptions& opt, bool paused = false) {
			play(channel, opt, paused);
		}

		inline void play(ChannelGroup& group, bool paused = false) {
			play(group, channel, paused);
		}

		inline void play(ChannelGroup& group, const SoundOptions& opt, bool paused = false) {
			play(group, channel, opt, paused);
		}

		constexpr Channel& getChannel() {
			return channel;
		}

		constexpr const Channel& getChannel() const {
			return channel;
		}

		void setDefaults(const SoundDefaults& defaults);
		SoundDefaults getDefaults() const;

		void set3DMinMaxDistance(const Sound3DMinMaxDistance& distance);
		Sound3DMinMaxDistance get3DMinMaxDistance() const;

		void set3DConeSettings(const ConeSettings& settings);
		ConeSettings get3DConeSettings() const;

		void set3DCustomRolloff(std::span<Vec3f> points);
		std::span<const Vec3f> get3DCustomRolloff() const;
		
		void getSubSound(int index, Sound& sound);
		void getSubSoundParent(Sound& parent);

		std::string getName() const;
		u32 getLength(TimeUnit unit = TimeUnit::Milliseconds) const;

		SoundFormatInfo getFormatInfo() const;

		inline SoundType getSoundType() const {
			return getFormatInfo().type;
		}

		inline SoundFormat getSoundFormat() const {
			return getFormatInfo().format;
		}

		inline u32 getSoundChannels() const {
			return getFormatInfo().channels;
		}

		inline u32 getSoundBits() const {
			return getFormatInfo().bits;
		}

		u32 getSubSoundCount() const;

		// getTagCount

		std::optional<FMOD_TAG> getTag(const std::string& name = "", int index = -1) const;
		
		SoundOpenStateInfo getOpenState() const;
		
		template<class T>
		u32 readData(std::span<T> buffer) {

			u32 read{};

			ASX_TRY() = handle->readData(buffer.data(), buffer.size_bytes(), &read);

			return read;

		}

		void seekData(u32 offsetPCM);

		// setSoundGroup
		// getSoundGroup

		u32 getSyncPointCount() const;

		// getSyncPoint
		// getSyncPointInfo
		// addSyncPoint
		// deleteSyncPoint

		// setMode
		// getMode

		void setLoopCount(u32 count = 0);
		u32 getLoopCount() const;

		void setLoopPoints(const TimePoint& start, const TimePoint& end);
		LoopPoints getLoopPoints(TimeUnit startUnit = TimeUnit::Milliseconds, TimeUnit endUnit = TimeUnit::Milliseconds);

		// getMusicChannelCount
		// setMusicChannelVolume
		// getMusicChannelVolume
		// setMusicSpeed
		// getMusicSpeed

	private:

		void playImpl(bool paused, FMOD::ChannelGroup* group = nullptr, FMOD::Channel** channel = nullptr);

		void playImpl(const SoundOptions& opt, bool paused, FMOD::ChannelGroup* group = nullptr, FMOD::Channel** channel = nullptr);

		Channel channel;
		bool nonBlocking = false;

	};


}
