/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 system.hpp
 */

#pragma once
#include "common.hpp"
#include "exception.hpp"


// ASX - Arclight FMOD wrapper
namespace ASX
{
	// Structures

	struct System3DOptions
	{
		float dopplerScale = 1.0f;
		float distanceFactor = 1.0f;
		float rolloffScale = 1.0f;
	};

	struct SpeakerPosition
	{
		float x, y;
		bool active;
	};

	struct ListenerAttributes
	{
		Vec3f position;
		Vec3f velocity;
		Vec3f forward = Vec3f(0, 0, 1);
		Vec3f up = Vec3f(0, 1, 0);
	};

	struct SystemChannelsPlaying
	{
		u32 channels;
		u32 realChannels;
	};

	// FMOD system user interface
	class ISystemUser
	{
	public:

		constexpr ISystemUser() :
			system(nullptr)
		{}

		constexpr ISystemUser(ISystemUser&& other) :
			system(std::exchange(other.system, nullptr))
		{}

		constexpr ISystemUser& operator=(ISystemUser&& other) {
			system = std::exchange(other.system, nullptr);
			return *this;
		}

		// Disable copy construction and assignment
		ISystemUser(ISystemUser&) = delete;
		ISystemUser& operator=(ISystemUser&) = delete;

		constexpr System* getSystem() {
			return system;
		}

	protected:

		constexpr FMOD::System* getSystemHandle();

		friend class System;
		System* system;

	};

	class System : public IHandleOwner<FMOD::System>
	{
	public:

		bool initialize();

		// Call once per frame to update 3D sound and callbacks
		void update();
		
		void shutdown();

		bool initialized() const;

		// Tie an object (Sound, DSP, ChannelGroup) to this system before creating it
		void tie(ISystemUser& user);

		template<class... Users>
		void tie(ISystemUser& user, Users&& ...users) {
			tie(user), (tie(users), ...);
		}

		/*
		*	Plugin support
		*/

		u32 loadPlugin(const Path& path);

		u32 registerCodec(CodecDescription& description, u32 priority = 0);
		
		/*
		*	General functions
		*/

		void setSpeakerPosition(Speaker speaker, const SpeakerPosition& position);
		SpeakerPosition getSpeakerPosition(Speaker speaker) const;

		void setStreamBufferSize(const TimePoint& size);
		TimePoint getStreamBufferSize() const;

		void set3DOptions(const System3DOptions& opt);
		System3DOptions get3DOptions() const;

		void set3DListenerCount(u32 count);
		u32 get3DListenerCount() const;

		void set3DListenerAttributes(u32 listener, const ListenerAttributes& attributes);
		ListenerAttributes get3DListenerAttributes(u32 listener) const;

		// set3DRolloffCallback

		void mixerSuspend();
		void mixerResume();

		// getDefaultMixMatrix

		u32 getSpeakerModeChannels(SpeakerMode mode) const;

		/*
		*	System information
		*/

		u32 getVersion() const;
		void* getOutputHandle() const;
		u32 getChannelsPlaying() const;
		u32 getRealChannelsPlaying() const;
		CPUUsage getCPUUsage() const;
		FileUsage getFileUsage() const;

		// createSound			-> Sound::create
		// createStream			-> Sound::create		(????)
		// createDSP			-> DSP::create
		// createDSPByType		-> DSP::create
		// createChannelGroup	-> ChannelGroup::create	(TODO)
		// createSoundGroup		-> SoundGroup::create	(TODO)
		// createReverb3D		-> Reverb3D::create		(TODO)

		// playSound			-> Sound::play
		// playDSP				-> DSP::play			(TODO)
		
		void getChannel(u32 id, Channel& channel) const;
		
		const DSPDescription& getDSPInfoByType(DSPType type) const;

		void getMasterChannelGroup(ChannelGroup& group);

		// getMasterSoundGroup(SoundGroup& group);

	private:

		friend class ISystemUser;

	};

	constexpr FMOD::System* ISystemUser::getSystemHandle() {
		return system ? system->handle : nullptr;
	}

}
