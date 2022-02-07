/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 sprite.hpp
 */

#pragma once

#include "springobject.hpp"
#include "spriteoutline.hpp"
#include "math/matrix.hpp"



class Sprite : private SpringObject {

public:

	constexpr Sprite() noexcept : spriteID(-1), rotation(0), scale(1, 1) {}


	//Getters
	constexpr float getX() const noexcept {
		return position.x;
	}

	constexpr float getY() const noexcept {
		return position.y;
	}

	constexpr const Vec2f& getPosition() const noexcept {
		return position;
	}

	constexpr float getRotation() const noexcept {
		return rotation;
	}

	constexpr float getScaleX() const noexcept {
		return scale.x;
	}

	constexpr float getScaleY() const noexcept {
		return scale.y;
	}

	constexpr const Vec2f& getScale() const noexcept {
		return scale;
	}

	constexpr float getShearX() const noexcept {
		return shear.x;
	}

	constexpr float getShearY() const noexcept {
		return shear.y;
	}

	constexpr const Vec2f& getShear() const noexcept {
		return shear;
	}


	//Setters
	constexpr void setX(float x) noexcept {
		position.x = x;
	}

	constexpr void setY(float y) noexcept {
		position.y = y;
	}

	constexpr void setPosition(float x, float y) noexcept {
		setPosition(Vec2f(x, y));
	}

	constexpr void setPosition(const Vec2f& pos) noexcept {
		position = pos;
	}

	constexpr void setRotation(float angle) noexcept {
		rotation = angle;
	}

	constexpr void setScaleX(float sx) noexcept {
		scale.x = sx;
	}

	constexpr void setScaleY(float sy) noexcept {
		scale.y = sy;
	}

	constexpr void setScale(float sx, float sy) noexcept {
		setScale(Vec2f(sx, sy));
	}

	constexpr void setScale(const Vec2f& s) noexcept {
		scale = s;
	}

	constexpr void setShearX(float sx) noexcept {
		shear.x = sx;
	}

	constexpr void setShearY(float sy) noexcept {
		shear.y = sy;
	}

	constexpr void setShear(float sx, float sy) noexcept {
		setShear(Vec2f(sx, sy));
	}

	constexpr void setShear(const Vec2f& s) noexcept {
		shear = s;
	}

private:

	friend class SpriteRenderer;

	u32 spriteID;

	Vec2f position;
	float rotation;
	Vec2f scale;
	Vec2f shear;

};