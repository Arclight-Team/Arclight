/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 dsp.hpp
 */

#pragma once
#include "common.hpp"
#include "exception.hpp"
#include "system.hpp"

#include <string>
#include <span>


// ASX - Arclight FMOD wrapper
namespace ASX
{
	// Enums

	enum class DSPConnectionType
	{
		Standard,
		Sidechain,
		Send,
		SendSidechain
	};

	enum class DSPType
	{
		Unknown,
		Mixer,
		Oscillator,
		Lowpass,
		ITLowpass,
		Highpass,
		Echo,
		Fader,
		Flange,
		Distortion,
		Normalize,
		Limiter,
		ParamEQ,
		PitchShift,
		Chorus,
		VSTPlugin,
		WinampPlugin,
		ITEcho,
		Compressor,
		SFXReverb,
		LowpassSimple,
		Delay,
		Tremolo,
		LADSPAPlugin,
		Send,
		Return,
		HighpassSimple,
		Pan,
		ThreeEQ,
		FFT,
		LoudnessMeter,
		EnvelopeFollower,
		ConvolutionReverb,
		ChannelMix,
		Transceiver,
		ObjectPan,
		MultibandEQ,
	};

	// Structures

	struct DSPChannelFormat
	{
		ChannelMask channelMask;
		SpeakerMode speakerMode;
		u32 channelCount;
	};

	// TODO: check correct default values
	struct DSPWetDryMix
	{
		float preWet	= 0.0f;
		float postWet	= 0.0f;
		float dry		= 0.0f;
	};

	struct DSPDescription : private FMOD_DSP_DESCRIPTION
	{

		void setName(const std::string& name) {
			std::strncpy(this->name, name.c_str(), 32);
		}

		std::string getName() {
			return name;
		}

	};

#define DEF_DSP_PARAMS(name, ...) struct name { enum : u32 { __VA_ARGS__ }; }

	// FMOD_DSP_CONVOLUTION_REVERB
	DEF_DSP_PARAMS(DSPConvolutionReverb, IR, Wet, Dry, Linked);

	// FMOD_DSP_ECHO
	DEF_DSP_PARAMS(DSPEcho, Delay, Feedback, Dry, Wet);

#undef DEF_DSP_PARAMS

	namespace Detail
	{
		template<DSPType Type>
		struct SelectDSPParameter
		{
			using ParamType = TT::NthPackType<SizeT(Type),
				void,					// Unknown
				void,					// Mixer
				void,					// Oscillator
				void,					// Lowpass
				void,					// ITLowpass
				void,					// Highpass
				DSPEcho,				// Echo
				void,					// Fader
				void,					// Flange
				void,					// Distortion
				void,					// Normalize
				void,					// Limiter
				void,					// ParamEQ
				void,					// PitchShift
				void,					// Chorus
				void,					// VSTPlugin
				void,					// WinampPlugin
				void,					// ITEcho
				void,					// Compressor
				void,					// SFXReverb
				void,					// LowpassSimple
				void,					// Delay
				void,					// Tremolo
				void,					// LADSPAPlugin
				void,					// Send
				void,					// Return
				void,					// HighpassSimple
				void,					// Pan
				void,					// ThreeEQ
				void,					// FFT
				void,					// LoudnessMeter
				void,					// EnvelopeFollower
				DSPConvolutionReverb,	// ConvolutionReverb
				void,					// ChannelMix
				void,					// Transceiver
				void,					// ObjectPan
				void					// MultibandEQ
			>;
		};

	}

	template<DSPType Type>
	using DSPParameters = Detail::SelectDSPParameter<Type>::ParamType;
	
	class DSP : public IHandleOwner<FMOD::DSP>, public ISystemUser
	{
	public:

		void create(DSPType type);
		void create(DSPDescription& description);
		void create(u32 pluginHandle);

		void destroy();

		/*
		*	Connection operations 
		*/

		void addInput(DSP& input, DSPConnection& connection, DSPConnectionType type = DSPConnectionType::Standard);
		void addInput(DSP& input, DSPConnectionType type = DSPConnectionType::Standard);
		
		void disconnectFrom(DSP& target, DSPConnection& connection);
		void disconnectFrom(DSP& target);

		u32 getInputCount() const;
		u32 getOutputCount() const;

		void getInput(u32 index, DSP& input, DSPConnection& connection);
		void getInput(u32 index, DSP& input);

		void getOutput(u32 index, DSP& output, DSPConnection& connection);
		void getOutput(u32 index, DSP& output);

		/*
		*	DSP unit control
		*/

		void setActive(bool active);
		bool isActive() const;

		inline void activate() {
			setActive(true);
		}

		inline void deactivate() {
			setActive(false);
		}

		void setBypass(bool bypass);
		bool isBypassed() const;

		void setWetDryMix(const DSPWetDryMix& mix);
		DSPWetDryMix getWetDryMix() const;

		void setChannelFormat(const DSPChannelFormat& format);
		DSPChannelFormat getChannelFormat() const;
		DSPChannelFormat getOutputChannelFormat(const DSPChannelFormat& inputFormat) const;

		/*
		*	DSP parameter control
		*/

		void setParameterFloat(u32 index, float value);
		void setParameterInt(u32 index, int value);
		void setParameterBool(u32 index, bool value);

		template<class T>
		void setParameterData(u32 index, std::span<T> buffer) {
			ASX_TRY() = handle->setParameterData(index, buffer.data(), buffer.size_bytes());
		}

		void showConfigDialog(void* hwnd, bool show);

		/*
		*	Getters 
		*/

	private:

		template<class T>
		friend class IChannelControl;
		friend class DSPConnection;

	};

	class DSPConnection : public IHandleOwner<FMOD::DSPConnection>
	{
	public:

		void getInput(DSP& dsp);
		void getOutput(DSP& dsp);

		void setMix(float volume);
		float getMix() const;

		void setMixMatrix(float* matrix, int outChannels, int inChannels, int inChannelHop = 0);
		float* getMixMatrix(int& outChannels, int& inChannels);

		template<int OutChannels, int InChannels>
		void setMixMatrix(float matrix[OutChannels][InChannels]) {
			setMixMatrix(matrix, OutChannels, InChannels);
		}

		DSPConnectionType getType() const;

	private:

		friend class DSP;

	};

}
