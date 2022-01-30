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
#include <variant>



enum class SpriteOutline {
	Square,
	Rectangle,
	Polygon
};



struct SpriteSquareOutline {

	constexpr SpriteSquareOutline() noexcept : SpriteSquareOutline(1.0) {}
	constexpr explicit SpriteSquareOutline(double size) noexcept : size(size) {}

	double size;

};



struct SpriteRectangleOutline {

	constexpr SpriteRectangleOutline() noexcept : SpriteRectangleOutline(Vec2d(1.0)) {}
	constexpr explicit SpriteRectangleOutline(const Vec2d& size) noexcept : size(size) {}

	Vec2d size;

};



struct SpritePolygonOutline {

	constexpr SpritePolygonOutline() noexcept = default;
	constexpr explicit SpritePolygonOutline(const std::span<const u8>& polygon) noexcept : polygon(polygon) {}

	std::span<const u8> polygon;

};



using SpriteOutlineData = std::variant<SpriteSquareOutline, SpriteRectangleOutline, SpritePolygonOutline>;

template<class T>
concept SpriteOutlineType = Equal<T, SpriteSquareOutline> || Equal<T, SpriteRectangleOutline> || Equal<T, SpritePolygonOutline>;