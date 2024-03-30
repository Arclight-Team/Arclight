/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 PixelFormat.hpp
 */

#pragma once

#include "FormatChannel.hpp"
#include "StdExt/BitSpan.hpp"

#include <numeric>



class PixelFormat {

public:

	constexpr PixelFormat() noexcept : channelCount(1), bytesPerPixel(1), channelType{Channel::Type::None}, channelBits{8} {}

	template<FormatChannel... F>
	constexpr static PixelFormat create() noexcept {

		arc_assert(sizeof...(F) <= 8, "Too many channels specified");

		std::vector<std::pair<u32, Channel>> channels;

		((F.bits ? void(channels.emplace_back(std::pair(F.offset, F))) : void()), ...);

		std::sort(channels.begin(), channels.end(), [](const auto& a, const auto& b) {
			return a.first < b.first;
		});

		std::vector<Channel> sortedChannels(channels.size());

		for (SizeT i = 0; i < channels.size(); i++) {
			sortedChannels[i] = channels[i].second;
		}

		PixelFormat format;
		format.create(sortedChannels);

		return format;

	}

	constexpr explicit PixelFormat(std::span<const Channel> channels) noexcept {
		create(channels);
	}

	constexpr u32 getChannelCount() const noexcept {
		return channelCount;
	}

	constexpr Channel::Type getChannelType(u32 channel) const noexcept {
		return getChannel(channel).type;
	}

	constexpr u8 getChannelBits(u32 channel) const noexcept {
		return getChannel(channel).bits;
	}

	constexpr u32 getBytesPerPixel() const noexcept {
		return bytesPerPixel;
	}

private:

	constexpr Channel getChannel(u32 channel) const noexcept {

		arc_assert(channel < channelCount, "Invalid channel index");

		return Channel(channelType[channel], channelBits[channel]);

	}

	constexpr void create(std::span<const Channel> channels) noexcept {

		arc_assert(channels.size() <= 8, "Too many channels specified");

		for (SizeT i = 0; i < channels.size(); i++) {

			channelType[i] = channels[i].type;
			channelBits[i] = channels[i].bits;

		}

		channelCount = channels.size();
		bytesPerPixel = Bits::ceilPowerOf2(std::accumulate(channels.begin(), channels.end(), 0U, [](u32 x, const Channel& channel) { return x + channel.bits; }), 8) / 8;

	}

	u32 bytesPerPixel;
	u32 channelCount;
	Channel::Type channelType[8];
	u8 channelBits[8];

};