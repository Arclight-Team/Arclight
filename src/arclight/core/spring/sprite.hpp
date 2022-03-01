/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 sprite.hpp
 */

#pragma once

#include "springobject.hpp"
#include "math/matrix.hpp"
#include "util/bitmaskenum.hpp"



class Sprite : private SpringObject {

public:

	constexpr Sprite() noexcept : typeID(-1), rotation(0), scale(1, 1) { setFlags(Created); }

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

	constexpr void setTypeID(u32 id) noexcept { typeID = id; typeDirty(); }
	constexpr void setGroupID(u32 id) noexcept { groupID = id; groupDirty(); }

	//Setters
	constexpr void setX(float x) noexcept { position.x = x; translationDirty(); }
	constexpr void setY(float y) noexcept { position.y = y; translationDirty(); }
	constexpr void setPosition(float x, float y) noexcept { setPosition(Vec2f(x, y)); }
	constexpr void setPosition(const Vec2f& pos) noexcept { position = pos; translationDirty(); }
	constexpr void setRotation(float angle) noexcept { rotation = angle; transformDirty(); }
	constexpr void setScaleX(float sx) noexcept { scale.x = sx; transformDirty(); }
	constexpr void setScaleY(float sy) noexcept { scale.y = sy; transformDirty(); }
	constexpr void setScale(float sx, float sy) noexcept { setScale(Vec2f(sx, sy)); }
	constexpr void setScale(const Vec2f& s) noexcept { scale = s; transformDirty(); }
	constexpr void setShearX(float sx) noexcept { shear.x = sx; transformDirty(); }
	constexpr void setShearY(float sy) noexcept { shear.y = sy; transformDirty(); }
	constexpr void setShear(float sx, float sy) noexcept { setShear(Vec2f(sx, sy)); }
	constexpr void setShear(const Vec2f& s) noexcept { shear = s; transformDirty(); }

private:

	friend class SpriteRenderer;

	constexpr void transformDirty() noexcept { setFlags(getFlags() | TransformDirty); }
	constexpr void translationDirty() noexcept { setFlags(getFlags() | TranslationDirty); }
	constexpr void groupDirty() noexcept { setFlags(getFlags() | GroupDirty); }
	constexpr void typeDirty() noexcept { setFlags(getFlags() | TypeDirty); }
	constexpr void allDirty() noexcept { setFlags(All); }

	enum Flags {
		Created = 0x1,
		TransformDirty = 0x2,
		TranslationDirty = 0x4,
		GroupDirty = 0x8,
		TypeDirty = 0x20,
		All = Created | TransformDirty | TranslationDirty | GroupDirty | TypeDirty
	};

	u64 id;
	u32 typeID;
	u32 groupID;
	u32 prevTypeID;
	u32 prevGroupID;

	Vec2f position;
	float rotation;
	Vec2f scale;
	Vec2f shear;

};