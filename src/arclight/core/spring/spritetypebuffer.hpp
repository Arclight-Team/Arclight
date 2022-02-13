/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritetypebuffer.hpp
 */

#pragma once

#include "memory/alignedallocator.hpp"
#include "types.hpp"

#include <memory>
#include <unordered_map>



struct SpriteType;

class SpriteTypeBuffer {

public:

	SpriteTypeBuffer();

	void addType(u32 typeID, const SpriteType& type);
	void setTypeData(u32 typeID, const SpriteType& type);
	void clear();

	u32 getTypeIndex(u32 typeID) const;

	void update();
	void bind();

private:

	constexpr static u32 typeDataSize = 32;

	void updateBounds(SizeT offset, SizeT size);
	void resetBounds();

	SizeT updateStart, updateEnd;

	std::vector<u8, AlignedAllocator<u8, 16>> buffer;
	std::shared_ptr<class SpriteTypeBufferData> data;
	std::unordered_map<u32, u32> typeMap;

};