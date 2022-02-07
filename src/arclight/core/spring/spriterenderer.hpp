/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriterenderer.hpp
 */

#pragma once

#include "spritearray.hpp"
#include "spritebatch.hpp"
#include "spritefactory.hpp"
#include "textureholder.hpp"
#include "math/rectangle.hpp"



class SpriteRenderer {

public:

	SpriteRenderer();

	void preload();
	void render();

	void setViewport(const Vec2d& lowerLeft, const Vec2d& topRight);

	//Sprite functions
	Sprite& createSprite(UUID id, UUID typeID);
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

	void recalculateProjection();

	TextureHolder textureHolder;
	SpriteFactory factory;
	SpriteArray sprites;
	std::unordered_map<u64, SpriteBatch> batches;

	RectD viewport;          //The scene's viewport rect
	Mat4f projection;

};