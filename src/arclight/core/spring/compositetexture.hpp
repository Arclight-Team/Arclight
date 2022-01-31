/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 compositetexture.hpp
 */

#pragma once

#include "util/bitmaskenum.hpp"

#include <vector>



class CompositeTexture {

public:

	enum class Type {
		Array = 0x1,
		Atlas = 0x2,
		AtlasArray = 0x4
	};

private:

	Type type;
	u32 width;
	u32 height;
	u32 length;
	bool present;
	std::vector<u8> pixels;

};


ARC_CREATE_BITMASK_ENUM(CompositeTexture::Type)