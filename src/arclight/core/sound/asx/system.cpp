/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 system.cpp
 */

#include "system.hpp"
#include "channel.hpp"
#include "util/log.hpp"


// ASX - Arclight FMOD wrapper
namespace ASX
{
	
	bool System::initialize() {
				
		if (initialized()) {
			Log::warn("ASS", "System already initialized");
			return true;
		}

		FMOD_RESULT res;

		handle = nullptr;

		ASX_TRY() = FMOD::System_Create(&handle);

		ASX_TRY() = handle->init(200, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, nullptr);

		return true;

	}

	void System::update() {

		if (!initialized()) {
			throw std::exception("System not initialized");
		}

		ASX_TRY() = handle->update();

	}

	void System::shutdown() {

		if (!initialized()) {
			Log::warn("ASS", "System not initialized");
			return;
		}

		ASX_TRY() = handle->close();
		ASX_TRY() = handle->release();
		handle = nullptr;

	}

	bool System::initialized() const  {
		return has();
	}

	void System::tie(ISystemUser& user) {

		if (user.system && user.system != this) {
			throw ASXException("System::tie() called on already tied object");
		}

		user.system = this;

	}

	/*
	*	Plugin support
	*/

	u32 System::loadPlugin(const Uri& path) {

		if (!initialized()) {
			throw std::exception("System not initialized");
		}

		if (!path.fileExists())
			return -1u;

		u32 pluginHandle;

		ASX_TRY() = handle->loadPlugin(path.getPath().c_str(), &pluginHandle);
		
		return pluginHandle;

	}

	/*
	*	General functions
	*/

	void System::setSpeakerPosition(Speaker speaker, const SpeakerPosition& position) {
		ASX_TRY() = handle->setSpeakerPosition(FMOD_SPEAKER(speaker), position.x, position.y, position.active);
	}

	SpeakerPosition System::getSpeakerPosition(Speaker speaker) const {

		SpeakerPosition position{};

		ASX_TRY() = handle->getSpeakerPosition(FMOD_SPEAKER(speaker), &position.x, &position.y, &position.active);
	
		return position;

	}

	void System::setStreamBufferSize(const TimePoint& size) {

		if (!initialized()) {
			throw std::exception("System not initialized");
		}

		ASX_TRY() = handle->setStreamBufferSize(size.value, size.getUnit());

	}

	TimePoint System::getStreamBufferSize() const {

		FMOD_TIMEUNIT type;
		u32 size;

		ASX_TRY() = handle->getStreamBufferSize(&size, &type);

		return { size, Conv::toTimeUnit(type) };

	}

	void System::set3DOptions(const System3DOptions& opt) {

		if (!initialized()) {
			throw std::exception("System not initialized");
		}

		ASX_TRY() = handle->set3DSettings(opt.dopplerScale, opt.distanceFactor, opt.rolloffScale);

	}

	System3DOptions System::get3DOptions() const {

		System3DOptions opt;

		if (!initialized()) {
			throw std::exception("System not initialized");
		}

		ASX_TRY() = handle->get3DSettings(&opt.dopplerScale, &opt.distanceFactor, &opt.rolloffScale);

		return opt;

	}

	void System::set3DListenerCount(u32 count) {
		ASX_TRY() = handle->set3DNumListeners(count);
	}

	u32 System::get3DListenerCount() const {

		int count{};

		ASX_TRY() = handle->get3DNumListeners(&count);

		return count;

	}

	void System::set3DListenerAttributes(u32 listener, const ListenerAttributes& attributes) {

		if (!initialized()) {
			throw std::exception("System not initialized");
		}

		auto& pos = castVec(attributes.position);
		auto& vel = castVec(attributes.velocity);
		auto& fwd = castVec(attributes.forward);
		auto& vup = castVec(attributes.up);

		ASX_TRY() = handle->set3DListenerAttributes(listener, &pos, &vel, &fwd, &vup);

	}

	ListenerAttributes System::get3DListenerAttributes(u32 listener) const {

		ListenerAttributes attributes;

		auto& pos = castVec(attributes.position);
		auto& vel = castVec(attributes.velocity);
		auto& fwd = castVec(attributes.forward);
		auto& vup = castVec(attributes.up);

		ASX_TRY() = handle->get3DListenerAttributes(listener, &pos, &vel, &fwd, &vup);

		return attributes;

	}

	void System::mixerSuspend() {
		ASX_TRY() = handle->mixerSuspend();
	}

	void System::mixerResume() {
		ASX_TRY() = handle->mixerResume();
	}

	u32 System::getSpeakerModeChannels(SpeakerMode mode) const {

		int count{};

		ASX_TRY() = handle->getSpeakerModeChannels(FMOD_SPEAKERMODE(mode), &count);

		return count;

	}

	/*
	*	System information
	*/

	u32 System::getVersion() const {

		u32 version;

		ASX_TRY() = handle->getVersion(&version);

		return version;

	}

	void* System::getOutputHandle() const {
		
		void* outputHandle{};

		ASX_TRY() = handle->getOutputHandle(&outputHandle);

		return outputHandle;

	}

	u32 System::getChannelsPlaying() const {

		int channels{};

		ASX_TRY() = handle->getChannelsPlaying(&channels);

		return channels;

	}

	u32 System::getRealChannelsPlaying() const {

		int channels{};

		ASX_TRY() = handle->getChannelsPlaying(nullptr, &channels);

		return channels;

	}

	CPUUsage System::getCPUUsage() const {

		FMOD_CPU_USAGE usage;
		CPUUsage cpu;

		ASX_TRY() = handle->getCPUUsage(&usage);

		cpu.dsp				= usage.dsp;
		cpu.stream			= usage.stream;
		cpu.geometry		= usage.geometry;
		cpu.update			= usage.update;
		cpu.convolution1	= usage.convolution1;
		cpu.convolution2	= usage.convolution2;

		return cpu;

	}

	FileUsage System::getFileUsage() const {

		FileUsage usage;

		ASX_TRY() = handle->getFileUsage(&usage.sampleBytesRead, &usage.streamBytesRead, &usage.otherBytesRead);

		return usage;

	}

	/*
	*	Object creation/retrieval
	*/

	void System::getChannel(u32 id, Channel& channel) const {

		FMOD::Channel* fc{};

		ASX_TRY() = handle->getChannel(id, &fc);

		channel.acquire(fc);

	}

	const DSPDescription& System::getDSPInfoByType(DSPType type) const {

		const FMOD_DSP_DESCRIPTION* desc;

		ASX_TRY() = handle->getDSPInfoByType(FMOD_DSP_TYPE(type), &desc);

		if (!desc)
			throw ASXException("Invalid DSP type specified");

		return *Bits::rcast<const DSPDescription*>(desc);

	}

	void System::getMasterChannelGroup(ChannelGroup& group) {

		if (!initialized()) {
			throw std::exception("System not initialized");
		}

		if (group.system && group.system != this)
			throw ASXException("Passed a ChannelGroup owned by a different System");
		else
			tie(group);

		FMOD::ChannelGroup* fcg{};

		ASX_TRY() = handle->getMasterChannelGroup(&fcg);

		group.acquire(fcg);

	}

}
