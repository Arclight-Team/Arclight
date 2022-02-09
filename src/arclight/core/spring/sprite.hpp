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
#include "util/bitmaskenum.hpp"



class Sprite : private SpringObject {

public:

	constexpr Sprite() noexcept : typeID(-1), rotation(0), scale(1, 1) { allDirty(); }

	//Getters
	constexpr float getX() const noexcept { return position.x; }
	constexpr float getY() const noexcept { return position.y; }
	constexpr const Vec2f& getPosition() const noexcept { return position; }
	constexpr float getRotation() const noexcept { return rotation; }
	constexpr float getScaleX() const noexcept { return scale.x; }
	constexpr float getScaleY() const noexcept { return scale.y; }
	constexpr const Vec2f& getScale() const noexcept { return scale; }
	constexpr float getShearX() const noexcept { return shear.x; }
	constexpr float getShearY() const noexcept { return shear.y; }
	constexpr const Vec2f& getShear() const noexcept { return shear; }

	constexpr u64 getID() const noexcept { return id; }
	constexpr u32 getTypeID() const noexcept { return typeID; }
	constexpr u32 getGroupID() const noexcept { return groupID; }

	//Setters
	constexpr void setX(float x) noexcept { position.x = x; translationDirty(); }
	constexpr void setY(float y) noexcept { position.y = y; translationDirty(); }
	constexpr void setPosition(float x, float y) noexcept { setPosition(Vec2f(x, y)); }
	constexpr void setPosition(const Vec2f& pos) noexcept { position = pos; translationDirty(); }
	constexpr void setRotation(float angle) noexcept { rotation = angle; rsTransformDirty(); }
	constexpr void setScaleX(float sx) noexcept { scale.x = sx; scaleDirty(); }
	constexpr void setScaleY(float sy) noexcept { scale.y = sy; scaleDirty(); }
	constexpr void setScale(float sx, float sy) noexcept { setScale(Vec2f(sx, sy)); }
	constexpr void setScale(const Vec2f& s) noexcept { scale = s; scaleDirty(); }
	constexpr void setShearX(float sx) noexcept { shear.x = sx; rsTransformDirty(); }
	constexpr void setShearY(float sy) noexcept { shear.y = sy; rsTransformDirty(); }
	constexpr void setShear(float sx, float sy) noexcept { setShear(Vec2f(sx, sy)); }
	constexpr void setShear(const Vec2f& s) noexcept { shear = s; rsTransformDirty(); }

private:

	friend class SpriteRenderer;

	constexpr void rsTransformDirty() noexcept { setFlags(getFlags() | Dirty | RSTransformDirty); }
	constexpr void translationDirty() noexcept { setFlags(getFlags() | Dirty | TranslationDirty); }
	constexpr void scaleDirty() noexcept { setFlags(getFlags() | Dirty | ScaleDirty); }
	constexpr void groupDirty() noexcept { setFlags(getFlags() | Dirty | GroupDirty); }
	constexpr void allDirty() noexcept { setFlags(All); }

	enum Flags {
		Dirty = 0x1,
		RSTransformDirty = 0x2,
		TranslationDirty = 0x4,
		ScaleDirty = 0x8,
		GroupDirty = 0x10,
		All = Dirty | RSTransformDirty | TranslationDirty | ScaleDirty | GroupDirty
	};

	u64 id;
	u32 typeID;
	u32 groupID;

	Vec2f position;
	float rotation;
	Vec2f scale;
	Vec2f shear;

};