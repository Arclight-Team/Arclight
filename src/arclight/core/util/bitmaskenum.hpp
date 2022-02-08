/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bitmaskenum.hpp
 */

#pragma once

#include "typetraits.hpp"
#include "concepts.hpp"



#define ARC_CREATE_BITMASK_ENUM(E)                                                                                      \
																														\
	static_assert(ScopedEnum<E>, "E must be a scoped enum type");                                                       \
																														\
	constexpr static E operator&(E a, E b) noexcept {                                                                   \
		return static_cast<E>(static_cast<TT::Underlying<E>>(a) & static_cast<TT::Underlying<E>>(b));                   \
	}                                                                                                                   \
																														\
	constexpr static E operator|(E a, E b) noexcept {                                                                   \
		return static_cast<E>(static_cast<TT::Underlying<E>>(a) | static_cast<TT::Underlying<E>>(b));                   \
	}                                                                                                                   \
																														\
	constexpr static E operator^(E a, E b) noexcept {                                                                   \
		return static_cast<E>(static_cast<TT::Underlying<E>>(a) ^ static_cast<TT::Underlying<E>>(b));                   \
	}                                                                                                                   \
																														\
	constexpr static E operator~(E a) noexcept {                                                                        \
		return static_cast<E>(~static_cast<TT::Underlying<E>>(a));                                                      \
	}                                                                                                                   \
																														\
	constexpr static E& operator&=(E& a, E b) noexcept {                                                                \
		a = a & b;                                                                                                      \
		return a;                                                                                                       \
	}                                                                                                                   \
																														\
	constexpr static E& operator|=(E& a, E b) noexcept {                                                                \
		a = a | b;                                                                                                      \
		return a;                                                                                                       \
	}                                                                                                                   \
																														\
	constexpr static E& operator^=(E& a, E b) noexcept {                                                                \
		a = a ^ b;                                                                                                      \
		return a;                                                                                                       \
	}


#define ARC_CREATE_MEMBER_BITMASK_ENUM(E)                                                                               \
																														\
	static_assert(ScopedEnum<E>, "E must be a scoped enum type");                                                       \
																														\
	constexpr friend E operator&(E a, E b) noexcept {                                                                   \
		return static_cast<E>(static_cast<TT::Underlying<E>>(a) & static_cast<TT::Underlying<E>>(b));                   \
	}                                                                                                                   \
																														\
	constexpr friend E operator|(E a, E b) noexcept {                                                                   \
		return static_cast<E>(static_cast<TT::Underlying<E>>(a) | static_cast<TT::Underlying<E>>(b));                   \
	}                                                                                                                   \
																														\
	constexpr friend E operator^(E a, E b) noexcept {                                                                   \
		return static_cast<E>(static_cast<TT::Underlying<E>>(a) ^ static_cast<TT::Underlying<E>>(b));                   \
	}                                                                                                                   \
																														\
	constexpr friend E operator~(E a) noexcept {                                                                        \
		return static_cast<E>(~static_cast<TT::Underlying<E>>(a));                                                      \
	}                                                                                                                   \
																														\
	constexpr friend E& operator&=(E& a, E b) noexcept {                                                                \
		a = a & b;                                                                                                      \
		return a;                                                                                                       \
	}                                                                                                                   \
																														\
	constexpr friend E& operator|=(E& a, E b) noexcept {                                                                \
		a = a | b;                                                                                                      \
		return a;                                                                                                       \
	}                                                                                                                   \
																														\
	constexpr friend E& operator^=(E& a, E b) noexcept {                                                                \
		a = a ^ b;                                                                                                      \
		return a;                                                                                                       \
	}
