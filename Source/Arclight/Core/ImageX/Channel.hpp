/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Channel.hpp
 */

#pragma once



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