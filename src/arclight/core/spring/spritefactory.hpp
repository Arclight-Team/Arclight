/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriteregistry.hpp
 */

#pragma once

#include "math/rectangle.hpp"
#include "spritetype.hpp"
#include "types.hpp"

#include <unordered_map>



class SpriteFactory {

public:

	const SpriteType& create(u32 id, u32 textureID, const Vec2f& size, const Vec2f& origin, const SpriteOutline& outline) {
		return spriteTypes.try_emplace(id, textureID, origin, size, outline).first->second;
	}

	bool contains(u32 id) const;
	const SpriteType& get(u32 id) const;

	void destroy(u32 id);
	void destroyAll();

private:

	constexpr static SpriteType defaultSpriteType;
	std::unordered_map<u32, SpriteType> spriteTypes;

};