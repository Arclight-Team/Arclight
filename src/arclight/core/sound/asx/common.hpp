/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 common.hpp
 */

#pragma once
#include "types.hpp"
#include "math/vector.hpp"
#include "util/bitmaskenum.hpp"
#include "util/bits.hpp"
#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include <fmod.hpp>


// ASX - Arclight FMOD wrapper
namespace ASX
{
	// FMOD result type
	using Result = FMOD_RESULT;

	// Core types forward declarations

	class DSP;
	class DSPConnection;

	class Sound;
	class SoundGroup;

	class Channel;
	class ChannelGroup;

	class System;

	// Enums

	enum class TimeUnit
	{
		Milliseconds,
		PCM,
		PCMBytes,
		Bytes
	};

	enum class LoopMode
	{
		Default,	// Might be none
		None,
		Forward,
		PingPong,
	};

	enum class OpenMode
	{
		Default,
		User,
		Memory,
		Raw,
		OpenOnly,
		MemoryPoint,
	};

	enum class CreateMode
	{
		Default,
		Stream,
		Sample,
		SampleCompressed,
	};

	enum class ChannelMask
	{
		FrontLeft				= 1 << 0,
		FrontRight				= 1 << 1,
		FrontCenter				= 1 << 2,
		LowFrequency			= 1 << 3,
		SurroundLeft			= 1 << 4,
		SurroundRight			= 1 << 5,
		BackLeft				= 1 << 6,
		BackRight				= 1 << 7,
		BackCenter				= 1 << 8,

		Mono					= FrontLeft,
		Stereo					= FrontLeft | FrontRight,
		LRC						= Stereo | FrontCenter,
		Quad					= Stereo | SurroundLeft | SurroundRight,
		Surround				= Quad | FrontCenter,
		Surround5Point1			= LRC | LowFrequency | SurroundLeft | SurroundRight,
		Surround5Point1Rears	= LRC | LowFrequency | BackLeft | BackRight,
		Surround7Point0			= Surround | BackLeft | BackRight,
		Surround7Point1			= Surround7Point0 | LowFrequency
	};

	enum class SpeakerMode
	{
		Default,
		Raw,
		Mono,
		Stereo,
		Quad,
		Surround,
		Surround5Point1,
		Surround7Point1,
		Surround7Point1Point4
	};

	enum class Speaker
	{
		None = -1,
		FrontLeft,
		FrontRight,
		FrontCenter,
		LowFrequency,
		SurroundLeft,
		SurroundRight,
		BackLeft,
		BackRight,
		TopFrontLeft,
		TopFrontRight,
		TopBackLeft,
		TopBackRight,
	};

	enum class SoundType
	{
		Unknown,
		AIFF,
		ASF,
		DLS,
		FLAC,
		FSB,
		IT,
		MIDI,
		MOD,
		MPEG,
		OGGVorbis,
		Playlist,
		RAW,
		S3M,
		USER,
		WAV,
		XM,
		XMA,
		AudioQueue,
		AT9,
		VORBIS,
		MediaFoundation,
		MediaCodec,
		FADPCM,
		OPUS
	};

	enum class SoundFormat
	{
		None,
		PCM8,
		PCM16,
		PCM24,
		PCM32,
		PCMFloat,
		Bitstream
	};

	enum class SoundOpenState
	{
		Ready,
		Loading,
		Error,
		Connecting,
		Buffering,
		Seeking,
		Playing,
		SetPosition
	};

	enum class SpaceRelative
	{
		HeadRelative,
		WorldRelative
	};

	enum class RolloffType
	{
		InverseRolloff,
		LinearRolloff,
		LinearSquareRolloff,
		InverseTaperedRolloff,
		CustomRolloff
	};

	enum class DSPType;

	// FMOD flags conversion functions
	namespace Conv
	{
		// Convert FMOD flag <-> TimeUnit
		constexpr TimeUnit toTimeUnit(u32 flag) {

			TimeUnit unit = TimeUnit::Milliseconds;

			// Select loop mode
			for (i32 f = u32(TimeUnit::Bytes); f >= u32(TimeUnit::Milliseconds); f--) {
				if (flag & (FMOD_TIMEUNIT_MS << f)) {
					unit = TimeUnit(f);
					break;
				}
			}

			return unit;


		}

		constexpr u32 fromTimeUnit(TimeUnit unit) {

			u32 flag;

			switch (unit) {

			case TimeUnit::Milliseconds:
				flag = FMOD_TIMEUNIT_MS;
				break;

			case TimeUnit::PCM:
				flag = FMOD_TIMEUNIT_PCM;
				break;

			case TimeUnit::PCMBytes:
				flag = FMOD_TIMEUNIT_PCMBYTES;
				break;

			case TimeUnit::Bytes:
				flag = FMOD_TIMEUNIT_RAWBYTES;
				break;

			}

			return flag;

		}

		// Convert FMOD flag <-> LoopMode
		constexpr LoopMode toLoopMode(u32 mode) {

			LoopMode loop = LoopMode::None;

			// Select loop mode
			for (i32 f = u32(LoopMode::PingPong); f >= u32(LoopMode::None); f--) {
				if (mode & (FMOD_LOOP_OFF << f)) {
					loop = LoopMode(f);
					break;
				}
			}

			return loop;

		}

		constexpr u32 fromLoopMode(LoopMode loop) {

			u32 mode;

			switch (loop) {

			case LoopMode::Default:
				mode = FMOD_DEFAULT;
				break;

			case LoopMode::None:
				mode = FMOD_LOOP_OFF;
				break;

			case LoopMode::Forward:
				mode = FMOD_LOOP_NORMAL;
				break;

			case LoopMode::PingPong:
				mode = FMOD_LOOP_BIDI;
				break;

			}

			return mode;

		}

		// Convert FMOD flag <-> OpenMode
		constexpr OpenMode toOpenMode(u32 mode) {

			if (mode & FMOD_OPENMEMORY && mode & FMOD_OPENMEMORY_POINT)
				return OpenMode::MemoryPoint;

			OpenMode open = OpenMode::Default;

			// Select open mode
			for (i32 f = u32(OpenMode::OpenOnly); f >= u32(OpenMode::User); f--) {
				if (mode & (FMOD_OPENUSER << f)) {
					open = OpenMode(f);
					break;
				}
			}

			return open;

		}

		constexpr u32 fromOpenMode(OpenMode open) {

			u32 mode;

			switch (open) {

			case OpenMode::Default:
				mode = FMOD_DEFAULT;
				break;

			case OpenMode::User:
				mode = FMOD_OPENUSER;
				break;

			case OpenMode::Memory:
				mode = FMOD_OPENMEMORY;
				break;

			case OpenMode::Raw:
				mode = FMOD_OPENRAW;
				break;

			case OpenMode::OpenOnly:
				mode = FMOD_OPENONLY;
				break;

			case OpenMode::MemoryPoint:
				mode = FMOD_OPENMEMORY | FMOD_OPENMEMORY_POINT;
				break;

			}

			return mode;

		}
		
		// Convert FMOD flag <-> CreateMode
		constexpr CreateMode toCreateMode(u32 mode) {

			CreateMode create = CreateMode::Default;

			// Select open mode
			for (i32 f = u32(CreateMode::SampleCompressed); f >= u32(CreateMode::Stream); f--) {
				if (mode & (FMOD_CREATESTREAM << f)) {
					create = CreateMode(f);
					break;
				}
			}

			return create;

		}

		constexpr u32 fromCreateMode(CreateMode create) {

			u32 mode;

			switch (create) {

			case CreateMode::Default:
				mode = FMOD_DEFAULT;
				break;

			case CreateMode::Stream:
				mode = FMOD_CREATESTREAM;
				break;

			case CreateMode::Sample:
				mode = FMOD_CREATESAMPLE;
				break;

			case CreateMode::SampleCompressed:
				mode = FMOD_CREATECOMPRESSEDSAMPLE;
				break;

			}

			return mode;

		}

		// Convert FMOD flag <-> 3D Relative
		constexpr SpaceRelative to3DRelative(u32 mode) {

			SpaceRelative relative = SpaceRelative::WorldRelative;

			// Select relative mode
			for (i32 f = i32(SpaceRelative::WorldRelative); f >= i32(SpaceRelative::HeadRelative); f--) {
				if (mode & (FMOD_3D_HEADRELATIVE << f)) {
					relative = SpaceRelative(f);
					break;
				}
			}

			return relative;

		}

		constexpr u32 from3DRelative(SpaceRelative relative) {

			u32 mode = 0;

			switch (relative) {

			case SpaceRelative::HeadRelative:
				mode |= FMOD_3D_HEADRELATIVE;
				break;

			case SpaceRelative::WorldRelative:
				mode |= FMOD_3D_WORLDRELATIVE;
				break;

			}

			return mode;

		}

		// Convert FMOD flag <-> 3D Rolloff
		constexpr RolloffType to3DRolloff(u32 mode) {

			RolloffType rolloff = RolloffType::InverseRolloff;

			// Select rolloff mode
			for (i32 f = i32(RolloffType::CustomRolloff); f >= i32(RolloffType::InverseRolloff); f--) {
				if (mode & (FMOD_3D_INVERSEROLLOFF << f)) {
					rolloff = RolloffType(f);
					break;
				}
			}

			return rolloff;

		}

		constexpr u32 from3DRolloff(RolloffType rolloff) {

			u32 mode = 0;

			switch (rolloff) {

			case RolloffType::InverseRolloff:
				mode |= FMOD_3D_INVERSEROLLOFF;
				break;

			case RolloffType::LinearRolloff:
				mode |= FMOD_3D_LINEARROLLOFF;
				break;

			case RolloffType::LinearSquareRolloff:
				mode |= FMOD_3D_LINEARSQUAREROLLOFF;
				break;

			case RolloffType::InverseTaperedRolloff:
				mode |= FMOD_3D_INVERSETAPEREDROLLOFF;
				break;

			case RolloffType::CustomRolloff:
				mode |= FMOD_3D_CUSTOMROLLOFF;
				break;

			}

			return mode;

		}

	}

	// TODO: ChannelControl
	using Rolloff3DCallback = float(*)(Channel& channelControl, float distance);

	// Structures

	struct CPUUsage
	{
		float dsp;
		float stream;
		float geometry;
		float update;
		float convolution1;
		float convolution2;
	};

	struct FileUsage
	{
		i64 sampleBytesRead;
		i64 streamBytesRead;
		i64 otherBytesRead;
	};

	struct TimePoint
	{
		constexpr TimePoint() = default;

		constexpr TimePoint(u32 value, TimeUnit unit = TimeUnit::Milliseconds) :
			value(value), unit(unit)
		{}

		constexpr u32 getUnit() const {
			return Conv::fromTimeUnit(unit);
		}

		constexpr void setUnit(u32 flag) {
			unit = Conv::toTimeUnit(flag);
		}

		constexpr static auto ms(u32 value) {
			return TimePoint(value, TimeUnit::Milliseconds);
		}

		constexpr static auto pcm(u32 value) {
			return TimePoint(value, TimeUnit::PCM);
		}

		constexpr static auto pcmBytes(u32 value) {
			return TimePoint(value, TimeUnit::PCMBytes);
		}

		constexpr static auto bytes(u32 value) {
			return TimePoint(value, TimeUnit::Bytes);
		}

		u32 value;
		TimeUnit unit;

	};

	struct SoundFormatInfo
	{
		SoundType type		= SoundType::Unknown;
		SoundFormat format	= SoundFormat::None;
		u32 channels		= 0;
		u32 bits			= 0;
	};

	struct OutputLevels
	{
		float frontLeft;
		float frontRight;
		float center;
		float lfe;
		float surroundLeft;
		float surroundRight;
		float backLeft;
		float backRight;
	};

	struct ConeSettings
	{
		float insideConeAngle = 360;
		float outsideConeAngle = 360;
		float outsideVolume = 1;
	};

	struct SpaceOptions3D
	{
		constexpr SpaceOptions3D() = default;

		constexpr SpaceOptions3D(u32 mode) {
			setFlag(mode);
		}

		constexpr u32 getFlag() const {

			if (!enabled)
				return FMOD_2D;

			u32 mode = FMOD_3D;

			if (ignoreGeometry)
				mode |= FMOD_3D_IGNOREGEOMETRY;

			mode |= Conv::from3DRelative(relative);
			mode |= Conv::from3DRolloff(rolloff);

			return mode;

		}

		constexpr void setFlag(u32 mode) {

			enabled = mode & FMOD_3D;
			ignoreGeometry = mode & FMOD_3D_IGNOREGEOMETRY;

			relative = Conv::to3DRelative(mode);
			rolloff = Conv::to3DRolloff(mode);

		}

		// Use 3D sound processing instead of 2D
		bool enabled = false;

		bool ignoreGeometry = false;

		// Relative to the listener or relative to the world
		SpaceRelative relative = SpaceRelative::WorldRelative;

		RolloffType rolloff = RolloffType::InverseRolloff;

	};
	
	struct ChannelControlOptions
	{
		constexpr ChannelControlOptions() = default;

		constexpr ChannelControlOptions(u32 mode) {
			setFlag(mode);
		}

		constexpr u32 getFlag() const {

			u32 mode = space3D.getFlag();

			mode |= Conv::fromLoopMode(loop);

			if (virtualPlayFromStart) {
				mode |= FMOD_VIRTUAL_PLAYFROMSTART;
			}

			return mode;

		}

		constexpr void setFlag(u32 mode) {

			space3D.setFlag(mode);

			loop = Conv::toLoopMode(mode);

			virtualPlayFromStart = mode & FMOD_VIRTUAL_PLAYFROMSTART;

		}

		// 3D space options
		SpaceOptions3D space3D;

		// Loop mode
		LoopMode loop = LoopMode::Default;

		// Virtual sounds will play from the start when becoming active again
		bool virtualPlayFromStart = false;

	};

	using ChannelOptions		= ChannelControlOptions;
	using ChannelGroupOptions	= ChannelControlOptions;
	
	struct SoundOptions
	{
		constexpr SoundOptions() = default;

		constexpr SoundOptions(u32 mode) {
			setFlag(mode);
		}

		constexpr u32 getFlag() const {

			u32 mode = space3D.getFlag();

			mode |= Conv::fromLoopMode(loop);
			mode |= Conv::fromOpenMode(open);
			mode |= Conv::fromCreateMode(create);

			if (nonBlocking)
				mode |= FMOD_NONBLOCKING;

			return mode;

		}

		constexpr void setFlag(u32 mode) {

			space3D.setFlag(mode);

			loop = Conv::toLoopMode(mode);
			open = Conv::toOpenMode(mode);
			create = Conv::toCreateMode(mode);

			nonBlocking = mode & FMOD_NONBLOCKING;

		}

		// 3D space options
		SpaceOptions3D space3D;

		// Loop mode
		LoopMode loop = LoopMode::Default;

		// Open mode
		OpenMode open = OpenMode::Default;

		// Create mode
		CreateMode create = CreateMode::Default;

		bool nonBlocking = false;

	};

	struct SoundOpenStateInfo
	{
		SoundOpenState openState;
		u32 percentBuffered;
		bool starving;
		bool diskBusy;
	};

	struct CreateSoundInfo : FMOD_CREATESOUNDEXINFO
	{
		// TODO!!
	};

	struct CodecDescription : FMOD_CODEC_DESCRIPTION {};

	struct ClockInterval
	{
		u64 start = 0;
		u64 end = 0;
	};

	struct LoopPoints
	{
		u32 start;
		u32 end;
	};

	struct FadePointInfo
	{
		u64 dspClock;
		float volume;
	};

	struct DSPDelayInfo : ClockInterval
	{
		bool stopChannels = true;
	};

	struct DSPClockInfo
	{
		u64 clock = 0;
		u64 parentClock = 0;
	};

	struct DSPDescription;

	namespace Detail
	{
		template<class V>
		struct ForwardType { using Type = V; };

		template<class V>
		struct VectorCastOutput {};
		
		template<> struct VectorCastOutput<Vec3f&>				: ForwardType<FMOD_VECTOR&>			{};
		template<> struct VectorCastOutput<Vec3f*>				: ForwardType<FMOD_VECTOR*>			{};
		template<> struct VectorCastOutput<const Vec3f&>		: ForwardType<const FMOD_VECTOR&>	{};
		template<> struct VectorCastOutput<const Vec3f*>		: ForwardType<const FMOD_VECTOR*>	{};
		template<> struct VectorCastOutput<FMOD_VECTOR&>		: ForwardType<Vec3f&>				{};
		template<> struct VectorCastOutput<FMOD_VECTOR*>		: ForwardType<Vec3f*>				{};
		template<> struct VectorCastOutput<const FMOD_VECTOR&>	: ForwardType<const Vec3f&>			{};
		template<> struct VectorCastOutput<const FMOD_VECTOR*>	: ForwardType<const Vec3f*>			{};

	}

	template<class V>
	using VectorCastOutput = Detail::VectorCastOutput<V>::Type;

	static_assert(CC::Equal<VectorCastOutput<Vec3f&>, FMOD_VECTOR&>);
	static_assert(CC::Equal<VectorCastOutput<const Vec3f&>, const FMOD_VECTOR&>);
	static_assert(CC::Equal<VectorCastOutput<Vec3f*>, FMOD_VECTOR*>);
	static_assert(CC::Equal<VectorCastOutput<const Vec3f*>, const FMOD_VECTOR*>);
	static_assert(CC::Equal<VectorCastOutput<FMOD_VECTOR&>, Vec3f&>);
	static_assert(CC::Equal<VectorCastOutput<const FMOD_VECTOR&>, const Vec3f&>);
	static_assert(CC::Equal<VectorCastOutput<FMOD_VECTOR*>, Vec3f*>);
	static_assert(CC::Equal<VectorCastOutput<const FMOD_VECTOR*>, const Vec3f*>);

	template<class In>
	auto castVec(In& vec) -> VectorCastOutput<In&> {
		return Bits::rcast<VectorCastOutput<In&>>(vec);
	}

	template<class In>
	auto castVec(In* vec) -> VectorCastOutput<In*> {
		return Bits::rcast<VectorCastOutput<In*>>(vec);
	}

	// Constants

	constexpr inline u32 LoopCountInfinite		= -1u;

	constexpr inline u32 DSPIndexHead			= -1u;
	constexpr inline u32 DSPIndexFader			= -2u;
	constexpr inline u32 DSPIndexTail			= -3u;

	// FMOD handle accessor interface
	template<class HandleT>
	class IHandleOwner
	{
	public:

		constexpr IHandleOwner() :
			handle(nullptr), userData(nullptr)
		{}

		constexpr IHandleOwner(IHandleOwner&& other) :
			handle(std::exchange(other.handle, nullptr)),
			userData(std::exchange(other.userData, nullptr))
		{}

		constexpr IHandleOwner& operator=(IHandleOwner&& other) {
			handle = std::exchange(other.handle, nullptr);
			userData = std::exchange(other.userData, nullptr);
			return *this;
		}

		// Disable copy construction and assignment
		IHandleOwner(IHandleOwner&) = delete;
		IHandleOwner& operator=(IHandleOwner&) = delete;

		void setUserData(void* data) {
			userData = data;
		}

		void* getUserData() {
			return userData;
		}

		const void* getUserData() const {
			return userData;
		}

		template<class T>
		T getUserData() {
			return Bits::rcast<T>(userData);
		}

		template<class T>
		const T getUserData() const {
			return Bits::rcast<const T>(userData);
		}

		constexpr bool has() const {
			return handle != nullptr;
		}

		constexpr operator bool() const {
			return has();
		}

	protected:

		friend class ISystemUser;

		HandleT* handle;
		void* userData;

	};

}

ARC_CREATE_BITMASK_ENUM(ASX::ChannelMask);
