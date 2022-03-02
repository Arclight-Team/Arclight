/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritetype.hpp
 */

#pragma once

#include "math/vector.hpp"

#include <span>



enum class SpriteOutline {
	Rectangle,
	Polygon
};


struct SpriteType {

	constexpr SpriteType() noexcept : SpriteType(0, Vec2f{}, Vec2f(1), Vec2f{}, Vec2f(1), SpriteOutline::Rectangle) {}
	constexpr SpriteType(u32 texture, const Vec2f& origin, const Vec2f& size, const Vec2f& uvBase, const Vec2f& uvScale, SpriteOutline outline, const std::span<const u8>& data = {}) noexcept
		: textureID(texture), origin(origin), size(size), uvBase(uvBase), uvScale(uvScale), outline(outline), polygon(data) {}

	u32 textureID;
	Vec2f origin;
	Vec2f size;
	Vec2f uvBase;
	Vec2f uvScale;
	SpriteOutline outline;
	std::span<const u8> polygon;

};