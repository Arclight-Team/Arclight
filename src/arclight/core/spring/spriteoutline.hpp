/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriteoutline.hpp
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