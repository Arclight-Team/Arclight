/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 PixelFormat.hpp
 */

#pragma once

#include "StdExt/BitSpan.hpp"
#include "Math/Saturate.hpp"

#include <numeric>



class PixelFormat {

public:

	struct Channel {

		enum class Type : u8 {
			None = 0,
			Unsigned,
			Float,
			Custom = 15
		};

		constexpr Channel() noexcept : type(Type::None), bits(0) {}
		constexpr Channel(Type type, u8 bits) noexcept : type(type), bits(bits) {}

		Type type;
		u8 bits;

	};

	template<Channel::Type T>
	struct FormatChannel : Channel {

		template<u32 B>
		using PixelDataType =	TT::Conditional<T == Type::Unsigned, TT::UnsignedFromMinSize<Bits::ceilPowerOf2(B, 8) / 8>,
								TT::Conditional<T == Type::Float && B == Bits::bitCount<float>(), float,
								TT::Conditional<T == Type::Float && B == Bits::bitCount<double>(), double,
								TT::UnsignedFromMinSize<Bits::ceilPowerOf2(B, 8) / 8>>>>;

		template<u32 B>
		struct Traits {

			using PixelType = PixelDataType<B>;

			constexpr static PixelType add(PixelType p, PixelType q) noexcept {

				if constexpr (T == Type::Unsigned) {

					constexpr bool Saturate = B > 4 && Bits::isPowerOf2(B);
					PixelType op = 0;

					if constexpr (Saturate) {

						op = Math::saturateAdd<PixelType>(p, q);

					} else {

						op = p + q;

						if (op > Bits::ones<PixelType>(B)) {
							op = Bits::ones<PixelType>(B);
						}

					}

					return op;

				} else if constexpr (T == Type::Float) {

					return p + q;

				}

				return 0;

			}

			constexpr static PixelType subtract(PixelType p, PixelType q) noexcept {

				if constexpr (T == Type::Float) {
					return p - q;
				} else if constexpr (T == Type::Unsigned) {
					return p >= q ? p - q : 0;
				}

				return 0;

			}

			template<CC::Arithmetic S>
			constexpr static PixelType multiply(PixelType p, S s) noexcept {

				if constexpr (T == Type::Float) {

					return p * s;

				} else if constexpr (T == Type::Unsigned) {

					constexpr bool Saturated = B > 4 && Bits::isPowerOf2(B);
					PixelType op = 0;

					if constexpr (CC::UnsignedType<S>) {

						op = Math::saturateMultiply<PixelType>(p, s);

						if (!Saturated) {

							if (op >= Bits::ones<PixelType>(B)) {
								op = Bits::ones<PixelType>(B);
							}

						}

					} else if constexpr (CC::SignedType<S>) {

						if (s < 0) {

							op = 0;

						} else {

							op = Math::saturateMultiply<PixelType>(p, s);

							if (!Saturated) {

								if (op >= Bits::ones<PixelType>(B)) {
									op = Bits::ones<PixelType>(B);
								}

							}

						}

					} else if constexpr (CC::Float<S>) {

						using OpType = TT::CommonType<PixelType, S>;

						OpType t = p * s;

						if (t < 0) {
							op = 0;
						} else if (t > Bits::ones<PixelType>(B)) {
							op = Bits::ones<PixelType>(B);
						} else {
							op = Math::round(t);
						}

					}

					return op;

				}

				return 0;

			}

			template<CC::Arithmetic S>
			constexpr static PixelType divide(PixelType p, S s) noexcept {

				if constexpr (T == Type::Float) {

					return p / s;

				} else if constexpr (T == Type::Unsigned) {

					if constexpr (CC::UnsignedType<S>) {

						return p / s;

					} else if constexpr (CC::SignedType<S>) {

						return s >= 0 ? p / s : 0;

					} else if constexpr (CC::Float<S>) {

						using OpType = TT::CommonType<PixelType, S>;

						OpType t = p / s;

						if (t < 0) {
							return 0;
						}

						if (t > Bits::ones<PixelType>(B)) {
							return Bits::ones<PixelType>(B);
						}

						return Math::round(t);

					}

				}

				return 0;

			}

		};

		constexpr static Type ChannelType = T;

		constexpr FormatChannel() noexcept : offset(0) {}

		constexpr FormatChannel(SizeT offset, SizeT bits) noexcept : Channel(T, bits), offset(offset) {}

		constexpr bool exists() const noexcept {
			return bits;
		}

		u32 offset;

	};

	using UnsignedChannel = FormatChannel<Channel::Type::Unsigned>;
	using FloatChannel = FormatChannel<Channel::Type::Float>;


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