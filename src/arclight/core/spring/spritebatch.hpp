/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritebatch.hpp
 */

#pragma once

#include "memory/alignedallocator.hpp"
#include "types.hpp"

#include <vector>



class SpriteBatch {

public:

	SpriteBatch() = default;

	u64 getKey() const noexcept;

	static u64 generateKey(u32 group, bool transparency, bool polygonal) noexcept;

private:

	u32 group;
	bool transparency;
	bool polygonal;
	//Texture?

	//std::vector<u8, AlignedAllocator<u8, 16>> vertexData;

};