/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriterenderer.hpp
 */

#pragma once

#include "spritearray.hpp"
#include "spritefactory.hpp"



class SpriteRenderer {

public:

	SpriteRenderer();

	/*
	 *  Sprite functions
	 */
	Sprite& createSprite(UUID id, UUID typeID, const Mat3d& transform = Mat3d());
	Sprite& getSprite(UUID id);
	const Sprite& getSprite(UUID id) const;
	bool containsSprite(UUID id) const;
	void destroySprite(UUID id);

	/*
	 *  Type functions
	 */
	template<SpriteOutlineType OutlineType>
	void createType(UUID id, UUID textureID, const Vec2d& relOrigin, const OutlineType& outline) {
		factory.create(id, textureID, relOrigin, outline);
	}

	bool hasType(UUID id) const;
	OptionalRef<const SpriteType> getType(UUID id) const;
	void destroyType(UUID id);

private:

	SpriteFactory factory;
	SpriteArray sprites;

};