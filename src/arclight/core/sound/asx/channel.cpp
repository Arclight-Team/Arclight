/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 channel.cpp
 */

#include "channel.hpp"
#include "util/assert.hpp"


// ASX - Arclight FMOD wrapper
namespace ASX
{

	/*
	*	Channel
	*/

	void Channel::destroy() {

		if (handle) {
			ASX_TRY() = handle->stop();

			release();
		}

	}

	void Channel::setFrequency(float frequency) {
		ASX_TRY() = handle->setFrequency(frequency);
	}

	float Channel::getFrequency() const {

		float frequency{};

		ASX_TRY() = handle->getFrequency(&frequency);

		return frequency;

	}

	void Channel::setPriority(i32 priority) {
		ASX_TRY() = handle->setPriority(priority);
	}

	i32 Channel::getPriority() {

		i32 priority{};

		ASX_TRY() = handle->getPriority(&priority);

		return priority;

	}

	void Channel::setPosition(const TimePoint& position) {
		ASX_TRY() = handle->setPosition(position.value, position.getUnit());
	}

	u32 Channel::getPosition(TimeUnit unit) {
		
		u32 position{};
		
		ASX_TRY() = handle->getPosition(&position, Conv::fromTimeUnit(unit));

		return position;

	}
	
	void Channel::setChannelGroup(ChannelGroup& channelGroup) {
		ASX_TRY() = handle->setChannelGroup(channelGroup.handle);
	}

	void Channel::setLoopCount(u32 count) {
		ASX_TRY() = handle->setLoopCount(count);
	}

	u32 Channel::setLoopCount() const {

		int count{};

		ASX_TRY() = handle->getLoopCount(&count);

		return count;

	}

	void Channel::setLoopPoints(const TimePoint& start, const TimePoint& end) {
		ASX_TRY() = handle->setLoopPoints(start.value, start.getUnit(), end.value, end.getUnit());
	}

	LoopPoints Channel::getLoopPoints(TimeUnit startUnit, TimeUnit endUnit) {

		u32 start{}, end{};

		ASX_TRY() = handle->getLoopPoints(&start, Conv::fromTimeUnit(startUnit), &end, Conv::fromTimeUnit(endUnit));

		return { start, end };

	}

	void Channel::setCallback(ChannelCallback function) {
		callback = function;
	}

	ChannelCallback Channel::getCallback() const {
		return callback;
	}

	void Channel::acquire(FMOD::Channel* channel) {

		release();

		handle = channel;

		if (channel) {
			channel->setUserData(this);
			channel->setCallback(callbackWrapper);
		}

	}

	void Channel::release() {

		if (handle) {
			handle->setUserData(nullptr);
			handle->setCallback(nullptr);
			handle = nullptr;
		}

	}

	FMOD_RESULT Channel::callbackWrapper(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2) {

		arc_assert(controltype == FMOD_CHANNELCONTROL_CHANNEL, "Invalid control type");

		auto fc = Bits::rcast<FMOD::Channel*>(channelcontrol);
		
		// Get FMOD::Channel user data
		void* ud = nullptr;
		fc->getUserData(&ud);

		auto c = static_cast<Channel*>(ud);

		if (!c->callback)
			return FMOD_OK;

		return c->callback(*c, ChannelCallbackType(callbacktype), AddressT(commanddata1), AddressT(commanddata2));

	}

	/*
	*	ChannelGroup
	*/

	void ChannelGroup::create(const std::string& name) {

		// Already constructed
		if (handle) {
			return;
		}

		FMOD::ChannelGroup* fcg{};

		ASX_TRY() = getSystemHandle()->createChannelGroup(name.c_str(), &fcg);

		acquire(fcg);

	}

	void ChannelGroup::destroy() {

		if (handle) {
			ASX_TRY() = handle->release();

			release();
		}

	}

	void ChannelGroup::addGroup(ChannelGroup& group, bool propagatedDSPClock) {
		ASX_TRY() = handle->addGroup(group.handle, propagatedDSPClock);
	}

	u32 ChannelGroup::getGroupCount() const {

		int count{};

		ASX_TRY() = handle->getNumGroups(&count);

		return count;

	}

	void ChannelGroup::getGroup(ChannelGroup& group, int index) const {

		FMOD::ChannelGroup* fcg{};

		ASX_TRY() = handle->getGroup(index, &fcg);

		group.acquire(fcg);

	}

	void ChannelGroup::getParentGroup(ChannelGroup& group) const {

		FMOD::ChannelGroup* fcg{};

		ASX_TRY() = handle->getParentGroup(&fcg);

		group.acquire(fcg);

	}

	std::string ChannelGroup::getName() const {

		std::string name;
		name.resize(1024);

		ASX_TRY() = handle->getName(name.data(), name.size());

		name.shrink_to_fit();

		return name;

	}

	u32 ChannelGroup::getChannelCount() const {

		int count{};

		ASX_TRY() = handle->getNumChannels(&count);

		return count;

	}

	void ChannelGroup::getChannel(Channel& channel, int index) {

		FMOD::Channel* fc{};

		ASX_TRY() = handle->getChannel(index, &fc);

		channel.acquire(fc);

	}

	void ChannelGroup::acquire(FMOD::ChannelGroup* group) {

		release();

		handle = group;

		if (group) {
			group->setUserData(this);
			group->setCallback(callbackWrapper);
		}

	}

	void ChannelGroup::release() {

		if (handle) {
			handle->setUserData(nullptr);
			handle->setCallback(nullptr);
			handle = nullptr;
		}

	}

	FMOD_RESULT ChannelGroup::callbackWrapper(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2) {

		arc_assert(controltype == FMOD_CHANNELCONTROL_CHANNELGROUP, "Invalid control type");

		auto fcg = Bits::rcast<FMOD::ChannelGroup*>(channelcontrol);

		// Get FMOD::Channel user data
		void* ud = nullptr;
		fcg->getUserData(&ud);

		auto cg = static_cast<ChannelGroup*>(ud);

		if (!cg->callback)
			return FMOD_OK;

		return cg->callback(*cg, ChannelCallbackType(callbacktype), AddressT(commanddata1), AddressT(commanddata2));

	}

}
