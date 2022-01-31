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
#include "textureholder.hpp"



class SpriteRenderer {

public:

	SpriteRenderer();

	void preload();
	void render();

	//Sprite functions
	Sprite& createSprite(UUID id, UUID typeID, const Mat3f& transform = Mat3f());
	Sprite& getSprite(UUID id);
	const Sprite& getSprite(UUID id) const;
	bool containsSprite(UUID id) const;
	void destroySprite(UUID id);

	//Type functions
	template<SpriteOutlineType OutlineType>
	void createType(UUID id, UUID textureID, const Vec2f& relOrigin, const OutlineType& outline) {
		factory.create(id, textureID, relOrigin, outline);
	}

	bool hasType(UUID id) const;
	OptionalRef<const SpriteType> getType(UUID id) const;
	void destroyType(UUID id);

	u32 activeSpriteCount() const noexcept;

private:

	TextureHolder textureHolder;
	SpriteFactory factory;
	SpriteArray sprites;

};