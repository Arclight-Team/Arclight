/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 channel.hpp
 */

#pragma once
#include "common.hpp"
#include "exception.hpp"
#include "dsp.hpp"
#include "system.hpp"


// ASX - Arclight FMOD wrapper
namespace ASX
{
	// Enums

	enum class ChannelCallbackType
	{
		End,
		VirtualVoice,
		SyncPoint,
		Occlusion
	};

	// Callback types

	using ChannelCallback = FMOD_RESULT(*)(Channel& channel, ChannelCallbackType callbackType, AddressT data1, AddressT data2);
	using ChannelGroupCallback = FMOD_RESULT(*)(ChannelGroup& channelGroup, ChannelCallbackType callbackType, AddressT data1, AddressT data2);

	// Structures

	template<class T>
	class IChannelControl : public IHandleOwner<T>
	{
		using Base = IHandleOwner<T>;

	public:

		/*
		*	Playback control
		*/

		bool isPlaying() const {

			bool playing{};
			
			// Ignore channels just stopped
			ASX_TRY(FMOD_ERR_INVALID_HANDLE) = Base::handle->isPlaying(&playing);

			return playing;

		}

		void stop() {
			ASX_TRY() = Base::handle->stop();
		}

		void setPaused(bool paused) {
			ASX_TRY() = Base::handle->setPaused(paused);
		}

		bool getPaused() const {

			bool paused{};

			ASX_TRY() = Base::handle->getPaused(&paused);

			return paused;

		}

		void setOptions(const ChannelControlOptions& opt) {
			ASX_TRY() = Base::handle->setMode(opt.getFlag());
		}

		void set3D(bool enabled = true) {

			u32 mode{};

			ASX_TRY() = Base::handle->getMode(&mode);

			// TODO!!!
			mode |= enabled ? FMOD_3D : FMOD_2D;

			ASX_TRY() = Base::handle->setMode(mode);

		}

		void set2D() {
			set3D(false);
		}

		void setLoop(LoopMode loop) {

			u32 mode{};

			ASX_TRY() = Base::handle->getMode(&mode);

			// TODO!!!
			mode |= Conv::fromLoopMode(loop);

			ASX_TRY() = Base::handle->setMode(mode);

		}

		ChannelControlOptions getOptions() const {
			
			ChannelControlOptions opt;

			u32 mode{};

			ASX_TRY() = Base::handle->getMode(&mode);

			opt.setFlag(mode);
			return opt;

		}

		bool is3D() const {

			u32 mode{};

			ASX_TRY() = Base::handle->getMode(&mode);

			return mode & FMOD_3D;

		}

		bool is2D() const {
			return !is3D();
		}

		LoopMode getLoop() const {

			u32 mode{};

			ASX_TRY() = Base::handle->getMode(&mode);

			return Conv::toLoopMode(mode);

		}

		void setPitch(float pitch) {
			ASX_TRY() = Base::handle->setPitch(pitch);
		}

		float getPitch() const {

			float pitch{};

			ASX_TRY() = Base::handle->getPitch(&pitch);

			return pitch;

		}

		/*
		*	Volume control
		*/

		float getAudibility() const {

			float audibility{};

			ASX_TRY() = Base::handle->getAudibility(&audibility);

			return audibility;

		}

		void setVolume(float volume) {
			ASX_TRY() = Base::handle->setVolume(volume);
		}

		float getVolume() const {

			float volume{};

			ASX_TRY() = Base::handle->getVolume(&volume);

			return volume;

		}

		void setVolumeRamped(bool ramp) {
			ASX_TRY() = Base::handle->setVolumeRamp(ramp);
		}

		bool isVolumeRamped() const {

			bool ramp{};

			ASX_TRY() = Base::handle->getVolumeRamp(&ramp);

			return ramp;

		}

		void setMute(bool mute) {
			ASX_TRY() = Base::handle->setMute(mute);
		}

		bool isMuted() const {

			bool mute{};

			ASX_TRY() = Base::handle->getMute(&mute);

			return mute;

		}

		/*
		*	Spatialization control
		*/

		void set3DAttributes(const Vec3f& position, const Vec3f& velocity) {

			auto& pos = castVec(position);
			auto& vel = castVec(velocity);

			ASX_TRY() = Base::handle->set3DAttributes(&pos, &vel);

		}

		void set3DPosition(const Vec3f& position) {

			auto& pos = castVec(position);

			ASX_TRY() = Base::handle->set3DAttributes(&pos, nullptr);

		}

		void set3DVelocity(const Vec3f& velocity) {

			auto& vel = castVec(velocity);

			ASX_TRY() = Base::handle->set3DAttributes(nullptr, &vel);

		}

		void get3DAttributes(Vec3f& position, Vec3f& velocity) const {

			auto& pos = castVec(position);
			auto& vel = castVec(velocity);

			ASX_TRY() = Base::handle->get3DAttributes(&pos, &vel);
		}

		Vec3f get3DPosition() const {

			Vec3f position;
			auto& pos = castVec(position);

			ASX_TRY() = Base::handle->get3DAttributes(&pos, nullptr);

			return position;

		}

		Vec3f get3DVelocity() const {

			Vec3f velocity;
			auto& vel = castVec(velocity);

			ASX_TRY() = Base::handle->get3DAttributes(nullptr, &vel);

			return velocity;

		}

		void set3DConeOrientation(Vec3f& orientation) {

			auto& vec = castVec(orientation);

			ASX_TRY() = Base::handle->set3DConeOrientation(&vec);

		}

		Vec3f get3DConeOrientation() const {
			
			Vec3f orientation;
			auto& vec = castVec(orientation);

			ASX_TRY() = Base::handle->get3DConeOrientation(&vec);

			return orientation;

		}

		void set3DConeSettings(const ConeSettings& settings) {
			ASX_TRY() = Base::handle->set3DConeSettings(settings.insideConeAngle, settings.outsideConeAngle, settings.outsideVolume);
		}

		ConeSettings get3DConeSettings() const {

			ConeSettings settings;

			ASX_TRY() = Base::handle->get3DConeSettings(&settings.insideConeAngle, &settings.outsideConeAngle, &settings.outsideVolume);

			return settings;

		}

		void set3DCustomRolloff(std::span<Vec3f> points) {

			auto list = castVec(points.data());
			auto size = points.size();

			ASX_TRY() = Base::handle->set3DCustomRolloff(list, size);

		}

		auto get3DCustomRolloff() const -> std::span<const Vec3f> {

			FMOD_VECTOR* points;
			int numpoints;

			ASX_TRY() = Base::handle->get3DCustomRolloff(&points, &numpoints);

			return { castVec(points), SizeT(numpoints) };

		}

		void reset3DDistanceFilter() {
			ASX_TRY() = Base::handle->set3DDistanceFilter(false, 0, 0);
		}

		void set3DDistanceFilter(float customLevel, float centerFrequency) {
			ASX_TRY() = Base::handle->set3DDistanceFilter(true, customLevel, centerFrequency);
		}

		void get3DDistanceFilter(float& customLevel, float& centerFrequency) const {

			bool custom = false;

			ASX_TRY() = Base::handle->get3DDistanceFilter(&custom, &customLevel, &centerFrequency);

			if (!custom)
				customLevel = 0;

		}

		void set3DDopplerLevel(float level) {
			ASX_TRY() = Base::handle->set3DDopplerLevel(level);
		}

		float get3DDopplerLevel() const {

			float level{};

			ASX_TRY() = Base::handle->get3DDopplerLevel(&level);

			return level;

		}

		void set3DLevel(float level) {
			ASX_TRY() = Base::handle->set3DLevel(level);
		}

		float get3DLevel() const {

			float level{};

			ASX_TRY() = Base::handle->get3DLevel(&level);

			return level;

		}

		void set3DMinMaxDistance(float min, float max) {
			ASX_TRY() = Base::handle->set3DMinMaxDistance(min, max);
		}

		void get3DMinMaxDistance(float& min, float& max) const {
			ASX_TRY() = Base::handle->get3DMinMaxDistance(&min, &max);
		}

		void set3DOcclusion(float directOcclusion, float reverbOcclusion) {
			ASX_TRY() = Base::handle->set3DOcclusion(directOcclusion, reverbOcclusion);
		}

		void get3DOcclusion(float& directOcclusion, float& reverbOcclusion) {
			ASX_TRY() = Base::handle->get3DOcclusion(&directOcclusion, &reverbOcclusion);
		}

		void set3DSpread(float angle) {
			ASX_TRY() = Base::handle->set3DSpread(angle);
		}

		float get3DSpread() const {

			float angle{};

			ASX_TRY() = Base::handle->get3DSpread(&angle);

			return angle;

		}

		/*
		*	Panning/level control
		*/

		void setPan(float pan) {
			ASX_TRY() = Base::handle->setPan(pan);
		}

		void setMixLevelsInput(std::span<float> levels) requires(Equal<T, FMOD::Channel>) {
			ASX_TRY() = Base::handle->setMixLevelsInput(levels.data(), levels.size());
		}

		void setMixLevelsOutput(const OutputLevels& levels) {
			ASX_TRY() = Base::handle->setMixLevelsOutput(levels.frontLeft, levels.frontRight, levels.center, levels.lfe, levels.surroundLeft, levels.surroundRight, levels.backLeft, levels.backRight);
		}

		void setMixMatrix(float* matrix, int outChannels, int inChannels, int inChannelHop = 0) {
			ASX_TRY() = Base::handle->setMixMatrix(matrix, outChannels, inChannels, inChannelHop);
		}

		template<int OutChannels, int InChannels>
		void setMixMatrix(float matrix[OutChannels][InChannels]) {
			setMixMatrix(matrix, OutChannels, InChannels);
		}

		float* getMixMatrix(int& outChannels, int& inChannels) {

			float* matrix{};

			ASX_TRY() = Base::handle->getMixMatrix(matrix, &outChannels, &inChannels);

			return matrix;

		}

		/*
		*	Filter control
		*/

		void setReverbProperties(int instance, float wet) {
			ASX_TRY() = Base::handle->setReverbProperties(instance, wet);
		}

		float getReverbProperties(int instance) const {

			float wet{};

			ASX_TRY() = Base::handle->getReverbProperties(instance, &wet);

			return wet;

		}

		void setLowPassGain(float gain) {
			ASX_TRY() = Base::handle->setLowPassGain(gain);
		}

		float getLowPassGain() const {

			float gain{};

			ASX_TRY() = Base::handle->getLowPassGain(&gain);

			return gain;

		}

		/*
		*	DSP chain configuration
		*/

		void getDSP(u32 index, DSP& dsp) {
			ASX_TRY() = Base::handle->getDSP(index, &dsp.handle);
		}

		void addDSP(u32 index, DSP& dsp) {
			ASX_TRY() = Base::handle->addDSP(index, dsp.handle);
		}

		void removeDSP(DSP& dsp) {
			ASX_TRY() = Base::handle->removeDSP(dsp.handle);
		}

		u32 getDSPCount() const {
			
			int count{};

			ASX_TRY() = Base::handle->getNumDSPs(&count);

			return count;

		}

		void setDSPIndex(DSP& dsp, u32 index) {
			ASX_TRY() = Base::handle->setDSPIndex(dsp.handle, index);
		}

		u32 getDSPIndex(DSP& dsp) const {

			int index{};

			ASX_TRY() = Base::handle->getDSPIndex(dsp.handle, &index);

			return index;

		}

		/*
		*	Sample accurate scheduling
		*/

		DSPClockInfo getDSPClock() const {

			DSPClockInfo info{};

			ASX_TRY() = Base::handle->getDSPClock(&info.clock, &info.parentClock);

			return info;

		}

		void setDelay(const DSPDelayInfo& info) {
			ASX_TRY() = Base::handle->setDelay(info.start, info.end, info.stopChannels);
		}

		DSPDelayInfo getDelay() const {

			DSPDelayInfo info{};

			ASX_TRY() = Base::handle->getDelay(&info.start, &info.end, &info.stopChannels);

			return info;

		}

		void addFadePoint(const FadePointInfo& info) {
			ASX_TRY() = Base::handle->addFadePoint(info.dspClock, info.volume);
		}

		void setFadePointRamp(const FadePointInfo& info) {
			ASX_TRY() = Base::handle->setFadePointRamp(info.dspClock, info.volume);
		}

		void removeFadePoints(const ClockInterval& interval) {
			ASX_TRY() = Base::handle->removeFadePoints(interval.start, interval.end);
		}

		u32 getFadePointCount() const {

			u32 count;

			ASX_TRY() = Base::handle->getFadePoints(&count, nullptr, nullptr);

			return count;

		}

		std::vector<FadePointInfo> getFadePoints() const {

			std::vector<FadePointInfo> points;

			std::vector<u64> clocks;
			std::vector<float> volumes;

			u32 count = getFadePointCount();

			clocks.resize(count);
			volumes.resize(count);

			ASX_TRY() = Base::handle->getFadePoints(&count, clocks.data(), volumes.data());

			for (u32 i = 0; i < count; i++) {
				points.push_back(FadePointInfo{ clocks[i], volumes[i] });
			}

			return points;

		}

		FadePointInfo getFadePoint(u32 index) const {

			if (index >= getFadePointCount())
				return {};

			auto points = getFadePoints();

			return points.at(index);

		}

	};

	class Channel : public IChannelControl<FMOD::Channel>
	{
	public:

		constexpr Channel() :
			callback(nullptr)
		{}

		inline ~Channel() {
			release();
		}

		void destroy();

		void setFrequency(float frequency);
		float getFrequency() const;

		void setPriority(i32 priority);
		i32 getPriority();

		void setPosition(const TimePoint& position);
		u32 getPosition(TimeUnit unit = TimeUnit::Milliseconds);
		
		void setChannelGroup(ChannelGroup& channelGroup);

		void setLoopCount(u32 count = 0);
		u32 setLoopCount() const;

		void setLoopPoints(const TimePoint& start, const TimePoint& end);
		LoopPoints getLoopPoints(TimeUnit startUnit = TimeUnit::Milliseconds, TimeUnit endUnit = TimeUnit::Milliseconds);

		void setCallback(ChannelCallback function);
		ChannelCallback getCallback() const;

	private:

		void acquire(FMOD::Channel* channel);
		void release();

		static FMOD_RESULT callbackWrapper(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2);

	private:

		friend class System;
		friend class Sound;
		friend class ChannelGroup;

		ChannelCallback callback;

	};

	class ChannelGroup : public IChannelControl<FMOD::ChannelGroup>, public ISystemUser
	{
	public:

		constexpr ChannelGroup() :
			callback(nullptr)
		{}

		inline ~ChannelGroup() {
			release();
		}

		void create(const std::string& name);

		void destroy();

		// TODO: DSPConnection** connection
		void addGroup(ChannelGroup& group, bool propagatedDSPClock = true);

		u32 getGroupCount() const;

		void getGroup(ChannelGroup& group, int index) const;

		void getParentGroup(ChannelGroup& group) const;

		std::string getName() const;

		u32 getChannelCount() const;

		void getChannel(Channel& channel, int index);

	private:

		void acquire(FMOD::ChannelGroup* group);
		void release();

		static FMOD_RESULT callbackWrapper(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2);

	private:

		friend class System;
		friend class Sound;
		friend class Channel;

		ChannelGroupCallback callback;

	};

}
