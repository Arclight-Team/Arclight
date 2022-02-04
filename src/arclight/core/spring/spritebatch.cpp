/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritebatch.cpp
 */

#include "spritebatch.hpp"




u64 SpriteBatch::getKey() const noexcept {
	return generateKey(group, transparency, polygonal);
}



u64 SpriteBatch::generateKey(u32 group, bool transparency, bool polygonal) noexcept {
	return u64(group) | u64(transparency) << 32 | u64(polygonal) << 33;
}