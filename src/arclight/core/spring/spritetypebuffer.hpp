/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritetypebuffer.hpp
 */

#pragma once

#include "syncbuffer.hpp"
#include "types.hpp"

#include <memory>
#include <unordered_map>



struct SpriteType;

class SpriteTypeBuffer {

public:

	SpriteTypeBuffer();

	void addType(u32 typeID, const SpriteType& type, u32 ctID, u32 texID);
	void setTypeData(u32 typeID, const SpriteType& type, u32 ctID, u32 texID);
	void clear();

	u32 getTypeIndex(u32 typeID) const;

	void update();
	void bind();

private:

	constexpr static u32 typeDataSize = 32;

	std::shared_ptr<class SpriteTypeBufferData> data;
	std::unordered_map<u32, u32> typeMap;

	SyncBuffer buffer;

};