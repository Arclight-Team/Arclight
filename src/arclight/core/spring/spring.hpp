/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spring.hpp
 */

#pragma once

#include "common/types.hpp"

#include <vector>



namespace Spring {

	constexpr u32 textureSlots = 16;
	constexpr u32 unusedCTSlot = -1;

	constexpr u32 baseShaderID = 0x00000080;
	constexpr u32 polygonalBit = 0x00000100;
	constexpr u32 transparencyBit = 0x00000200;

	constexpr u32 sharedBufferBinding = 0;
	constexpr u32 ctTableBufferBinding = 1;

	constexpr u32 typeBufferBinding = 0;

}