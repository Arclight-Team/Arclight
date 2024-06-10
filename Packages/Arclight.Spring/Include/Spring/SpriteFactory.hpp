/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SpriteRegistry.hpp
 */

#pragma once

#include "SpriteType.hpp"
#include "Common/Types.hpp"

#include <unordered_map>



class SpriteFactory {

public:

	const SpriteType& create(u32 id, u32 textureID, const Vec2f& size, const Vec2f& origin, const Vec2f& uvBase, const Vec2f& uvScale, SpriteOutline outline, const std::span<const u8>& polygon) {
		return spriteTypes.try_emplace(id, textureID, origin, size, uvBase, uvScale, outline, polygon).first->second;
	}

	bool contains(u32 id) const;
	const SpriteType& get(u32 id) const;

	void destroy(u32 id);
	void destroyAll();

private:

	std::unordered_map<u32, SpriteType> spriteTypes;

};