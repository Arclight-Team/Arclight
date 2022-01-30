/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriteregistry.hpp
 */

#pragma once

#include "spriteoutline.hpp"
#include "types.hpp"

#include <unordered_map>



struct SpriteType {

	constexpr SpriteType() noexcept : textureID(-1), outlineType(SpriteOutline::Square), outlineData(SpriteSquareOutline()) {}

	u32 textureID;
	Vec2d origin;
	SpriteOutline outlineType;
	SpriteOutlineData outlineData;

};



class SpriteFactory {

public:

	template<SpriteOutlineType OutlineType>
	void create(u64 id, u32 textureID, const Vec2d& relOrigin, const OutlineType& outline) {

		SpriteType type;
		type.textureID = textureID;
		type.origin = relOrigin;
		type.outlineData = outline;

		if constexpr (Equal<OutlineType, SpriteSquareOutline>) {
			type.outlineType = SpriteOutline::Square;
		} else if constexpr (Equal<OutlineType, SpriteRectangleOutline>) {
			type.outlineType = SpriteOutline::Rectangle;
		} else if constexpr (Equal<OutlineType, SpritePolygonOutline>) {
			type.outlineType = SpriteOutline::Polygon;
		} else {
			arc_force_assert("Unknown spring outline type");
		}

		spriteTypes[id] = type;

	}

	bool contains(u64 id) const;
	const SpriteType& get(u64 id) const;

	void destroy(u64 id);
	void destroyAll();

private:

	std::unordered_map<u64, SpriteType> spriteTypes;

};