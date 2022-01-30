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

	constexpr Sprite() noexcept = default;

private:

	friend class SpriteRenderer;

	u32 spriteID;
	Mat3d transform;

};