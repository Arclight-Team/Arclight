/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 FormatChannel.hpp
 */

#pragma once

#include "Channel.hpp"
#include "PixelTraits.hpp"



template<Channel::Type T, class PTraits = PixelTraits<T>>
struct FormatChannel : Channel {

	using Traits = PTraits;
	constexpr static Type ChannelType = T;

	constexpr FormatChannel() noexcept : offset(0) {}

	constexpr FormatChannel(SizeT offset, SizeT bits) noexcept : Channel(T, bits), offset(offset) {}

	constexpr bool exists() const noexcept {
		return bits;
	}

	u32 offset;

};



template<FormatChannel... F>
struct ChannelOrder {

	constexpr static std::array<u8, sizeof...(F)> Order = []() {

		std::array<std::tuple<u32, u8, bool>, sizeof...(F)> channels;
		SizeT i = 0;

		([&](auto& c, const auto& x) {
			std::get<0>(c[i]) = x.offset;
			std::get<1>(c[i]) = i;
			std::get<2>(c[i]) = x.exists();
			i++;
		}(channels, F), ...);

		//((channels[i] = std::tuple<u32, u8, bool>(F.offset, i++, F.exists())), ...);

		std::sort(channels.begin(), channels.end(), [](const auto& a, const auto& b) {
			return std::get<bool>(a) ? std::get<u32>(a) < std::get<u32>(b) : false;
		});

		std::array<u8, sizeof...(F)> order;

		for (u32 j = 0; j < channels.size(); j++) {
			order[std::get<u8>(channels[j])] = j;
		}

		return order;

	}();

	template<SizeT I>
	constexpr static SizeT Index = Order[I];

	template<SizeT I, class... T>
	constexpr static auto getValue(T... t) noexcept {

		std::tuple x = std::tie(t...);
		return std::get<Index<I>>(x);

	}

};

using UnsignedChannel = FormatChannel<Channel::Type::Unsigned>;
using FloatChannel = FormatChannel<Channel::Type::Float>;