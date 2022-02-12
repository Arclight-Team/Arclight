/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriteregistry.hpp
 */

#pragma once

#include "math/rectangle.hpp"
#include "spriteoutline.hpp"
#include "types.hpp"

#include <unordered_map>



struct SpriteType {

	constexpr SpriteType() noexcept : textureID(-1), size(Vec2f(1)) {}
	constexpr SpriteType(u32 texture, const Vec2f& origin, const Vec2f& size, const SpriteOutline& outline) noexcept : textureID(textureID), origin(origin), size(size), outline(outline) {}

	u32 textureID;
	Vec2f origin;
	Vec2f size;
	SpriteOutline outline;

};



class SpriteFactory {

public:

	void create(u64 id, u32 textureID, const Vec2f& size, const Vec2f& origin, const SpriteOutline& outline) {
		spriteTypes.try_emplace(id, textureID, origin, size, outline);
	}

	bool contains(u64 id) const;
	const SpriteType& get(u64 id) const;

	void destroy(u64 id);
	void destroyAll();

private:

	std::unordered_map<u64, SpriteType> spriteTypes;

};