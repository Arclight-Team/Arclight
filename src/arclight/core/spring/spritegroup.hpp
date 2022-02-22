/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritegroup.hpp
 */

#pragma once



class SpriteGroup {

public:

	SpriteGroup() : visible(true) {}

	constexpr bool isVisible() const { return visible; }

	constexpr void show() { visible = true; }
	constexpr void hide() { visible = false; }
	constexpr void setVisibility(bool visible) { this->visible = visible; }
	constexpr void toggleVisibility() { visible = !visible; }

private:

	bool visible;

};