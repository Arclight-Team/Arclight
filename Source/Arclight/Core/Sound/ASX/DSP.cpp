/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 DSP.cpp
 */

#include "DSP.hpp"


// ASX - Arclight FMOD wrapper
namespace ASX
{

	/*
	*	DSP
	*/

	void DSP::create(DSPType type) {

		// Already constructed
		if (handle)
			return;

		handle = nullptr;
		ASX_TRY() = getSystemHandle()->createDSPByType(FMOD_DSP_TYPE(type), &handle);

	}

	void DSP::create(DSPDescription& description) {

		auto desc = Bits::rcast<const FMOD_DSP_DESCRIPTION*>(&description);

		// Already constructed
		if (handle)
			return;

		handle = nullptr;
		ASX_TRY() = getSystemHandle()->createDSP(desc, &handle);

	}

	void DSP::create(u32 pluginHandle) {

		// Already constructed
		if (handle)
			return;

		handle = nullptr;
		ASX_TRY() = getSystemHandle()->createDSPByPlugin(pluginHandle, &handle);

	}

	void DSP::destroy() {

		if (handle) {
			ASX_TRY() = handle->release();
			handle = nullptr;
		}

	}

	/*
	*	Connection operations
	*/

	void DSP::addInput(DSP& input, DSPConnection& connection, DSPConnectionType type) {
		ASX_TRY() = handle->addInput(input.handle, &connection.handle, FMOD_DSPCONNECTION_TYPE(type));
	}

	void DSP::addInput(DSP& input, DSPConnectionType type) {
		ASX_TRY() = handle->addInput(input.handle, nullptr, FMOD_DSPCONNECTION_TYPE(type));
	}

	void DSP::disconnectFrom(DSP& target, DSPConnection& connection) {
		ASX_TRY() = handle->disconnectFrom(target.handle, connection.handle);
	}

	void DSP::disconnectFrom(DSP& target) {
		ASX_TRY() = handle->disconnectFrom(target.handle);
	}

	u32 DSP::getInputCount() const {

		int count{};

		ASX_TRY() = handle->getNumInputs(&count);

		return count;

	}

	u32 DSP::getOutputCount() const {

		int count{};

		ASX_TRY() = handle->getNumOutputs(&count);

		return count;

	}

	void DSP::getInput(u32 index, DSP& input, DSPConnection& connection) {
		ASX_TRY() = handle->getInput(index, &input.handle, &connection.handle);
	}

	void DSP::getInput(u32 index, DSP& input) {
		ASX_TRY() = handle->getInput(index, &input.handle, nullptr);
	}

	void DSP::getOutput(u32 index, DSP& output, DSPConnection& connection) {
		ASX_TRY() = handle->getOutput(index, &output.handle, &connection.handle);
	}

	void DSP::getOutput(u32 index, DSP& output) {
		ASX_TRY() = handle->getOutput(index, &output.handle, nullptr);
	}

	/*
	*	DSP unit control
	*/

	void DSP::setActive(bool active) {
		ASX_TRY() = handle->setActive(active);
	}

	bool DSP::isActive() const {

		bool active{};

		ASX_TRY() = handle->getActive(&active);

		return true;

	}

	void DSP::setBypass(bool bypass) {
		ASX_TRY() = handle->setBypass(bypass);
	}

	bool DSP::isBypassed() const {

		bool bypass{};

		ASX_TRY() = handle->getBypass(&bypass);

		return bypass;

	}

	void DSP::setWetDryMix(const DSPWetDryMix& mix) {
		ASX_TRY() = handle->setWetDryMix(mix.preWet, mix.postWet, mix.dry);
	}

	DSPWetDryMix DSP::getWetDryMix() const {

		DSPWetDryMix mix{};

		ASX_TRY() = handle->getWetDryMix(&mix.preWet, &mix.postWet, &mix.dry);
		return mix;

	}

	void DSP::setChannelFormat(const DSPChannelFormat& format) {

		auto mask = FMOD_CHANNELMASK(format.channelMask);
		auto mode = FMOD_SPEAKERMODE(format.speakerMode);
		int count = format.channelCount;

		ASX_TRY() = handle->setChannelFormat(mask, count, mode);

	}

	DSPChannelFormat DSP::getChannelFormat() const {

		FMOD_CHANNELMASK mask;
		FMOD_SPEAKERMODE mode;
		int count;

		ASX_TRY() = handle->getChannelFormat(&mask, &count, &mode);

		return { ChannelMask(mask), SpeakerMode(mode), u32(count) };

	}

	DSPChannelFormat DSP::getOutputChannelFormat(const DSPChannelFormat& inputFormat) const {

		auto mask = FMOD_CHANNELMASK(inputFormat.channelMask);
		auto mode = FMOD_SPEAKERMODE(inputFormat.speakerMode);
		int count = inputFormat.channelCount;

		FMOD_CHANNELMASK outMask;
		FMOD_SPEAKERMODE outMode;
		int outCount;

		ASX_TRY() = handle->getOutputChannelFormat(mask, count, mode, &outMask, &outCount, &outMode);

		return { ChannelMask(outMask), SpeakerMode(outMode), u32(outCount) };

	}

	/*
	*	DSP parameter control
	*/

	void DSP::setParameterFloat(u32 index, float value) {
		ASX_TRY() = handle->setParameterFloat(index, value);
	}

	void DSP::setParameterInt(u32 index, int value) {
		ASX_TRY() = handle->setParameterInt(index, value);
	}

	void DSP::setParameterBool(u32 index, bool value) {
		ASX_TRY() = handle->setParameterBool(index, value);
	}

	void DSP::showConfigDialog(void* hwnd, bool show) {
		ASX_TRY() = handle->showConfigDialog(hwnd, show);
	}

	float DSP::getParameterFloat(u32 index, std::string& string) const {
		
		float value{};

		string.resize(512);

		ASX_TRY() = handle->getParameterFloat(index, &value, string.data(), string.size());

		string.shrink_to_fit();

		return value;

	}

	int DSP::getParameterInt(u32 index, std::string& string) const {

		int value{};

		string.resize(512);

		ASX_TRY() = handle->getParameterInt(index, &value, string.data(), string.size());

		string.shrink_to_fit();

		return value;

	}

	bool DSP::getParameterBool(u32 index, std::string& string) const {

		bool value{};

		string.resize(512);

		ASX_TRY() = handle->getParameterBool(index, &value, string.data(), string.size());

		string.shrink_to_fit();

		return value;

	}

	std::span<const u8> DSP::getParameterData(u32 index, std::string& string) const {

		void* data{};
		u32 size{};

		string.resize(512);

		ASX_TRY() = handle->getParameterData(index, &data, &size, string.data(), string.size());

		string.shrink_to_fit();

		return std::span(static_cast<u8*>(data), size);

	}

	float DSP::getParameterFloat(u32 index) const {

		float value{};

		ASX_TRY() = handle->getParameterFloat(index, &value, nullptr, 0);

		return value;

	}

	int DSP::getParameterInt(u32 index) const {

		int value{};

		ASX_TRY() = handle->getParameterInt(index, &value, nullptr, 0);

		return value;

	}

	bool DSP::getParameterBool(u32 index) const {

		bool value{};

		ASX_TRY() = handle->getParameterBool(index, &value, nullptr, 0);

		return value;

	}

	std::span<const u8> DSP::getParameterData(u32 index) const {

		void* data{};
		u32 size{};

		ASX_TRY() = handle->getParameterData(index, &data, &size, nullptr, 0);

		return std::span(static_cast<u8*>(data), size);

	}

	u32 DSP::getParameterCount() const {

		int count{};

		ASX_TRY() = handle->getNumParameters(&count);

		return count;

	}

	const DSPParameterDescription& DSP::getParameterInfo(u32 index) const {

		FMOD_DSP_PARAMETER_DESC* desc{};

		ASX_TRY() = handle->getParameterInfo(index, &desc);

		// TODO: can this be nullptr or does it return an error?
		if (!desc)
			throw ASXException("Parameter not found");

		return *static_cast<const DSPParameterDescription*>(desc);

	}
	
	/*
	*	DSPConnection
	*/

	void DSPConnection::getInput(DSP& dsp) {
		ASX_TRY() = handle->getInput(&dsp.handle);
	}

	void DSPConnection::getOutput(DSP& dsp) {
		ASX_TRY() = handle->getOutput(&dsp.handle);
	}

	void DSPConnection::setMix(float volume) {
		ASX_TRY() = handle->setMix(volume);
	}

	float DSPConnection::getMix() const {

		float volume{};

		ASX_TRY() = handle->getMix(&volume);

		return volume;

	}

	void DSPConnection::setMixMatrix(float* matrix, int outChannels, int inChannels, int inChannelHop) {
		ASX_TRY() = handle->setMixMatrix(matrix, outChannels, inChannels, inChannelHop);
	}

	float* DSPConnection::getMixMatrix(int& outChannels, int& inChannels) {

		float* matrix{};

		ASX_TRY() = handle->getMixMatrix(matrix, &outChannels, &inChannels);

		return matrix;

	}

	DSPConnectionType DSPConnection::getType() const {

		FMOD_DSPCONNECTION_TYPE type;

		ASX_TRY() = handle->getType(&type);

		return DSPConnectionType(type);

	}

}
