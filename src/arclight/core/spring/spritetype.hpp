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



struct SpriteOutline {

	enum class Type {
		Rectangle,
		Polygon
	};

	constexpr SpriteOutline() : uvBase(Vec2f(0)), uvScale(Vec2f(1)), type(Type::Rectangle) {}
	constexpr SpriteOutline(const Vec2f& base, const Vec2f& scale, Type type, const std::span<const u8>& data) : uvBase(base), uvScale(scale), type(type), polygon(data) {}

	Vec2f uvBase;
	Vec2f uvScale;
	Type type;
	std::span<const u8> polygon;

};



struct SpriteType {

	constexpr SpriteType() noexcept : textureID(0), size(Vec2f(1)) {}
	constexpr SpriteType(u32 texture, const Vec2f& origin, const Vec2f& size, const SpriteOutline& outline) noexcept : textureID(textureID), origin(origin), size(size), outline(outline) {}

	u32 textureID;
	Vec2f origin;
	Vec2f size;
	SpriteOutline outline;

};