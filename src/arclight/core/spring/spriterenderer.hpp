/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriterenderer.hpp
 */

#pragma once

#include <map>
#include "spritearray.hpp"
#include "spritebatch.hpp"
#include "spritefactory.hpp"
#include "textureholder.hpp"
#include "math/rectangle.hpp"



class SpriteRendererShaders;

class SpriteRenderer {

public:

	SpriteRenderer();

	void preload();
	void render();

	void setViewport(const Vec2d& lowerLeft, const Vec2d& topRight);

	//Sprite functions
	Sprite& createSprite(Id64 id, Id32 typeID, Id32 groupID = 0);
	Sprite& getSprite(Id64 id);
	const Sprite& getSprite(Id64 id) const;
	bool containsSprite(Id64 id) const;
	void destroySprite(Id64 id);

	//Type functions
	template<SpriteOutlineType OutlineType>
	void createType(Id64 id, Id64 textureID, const Vec2f& relOrigin, const OutlineType& outline) {
		factory.create(id, textureID, relOrigin, outline);
	}

	bool hasType(Id64 id) const;
	OptionalRef<const SpriteType> getType(Id64 id) const;
	void destroyType(Id64 id);

	u32 activeSpriteCount() const noexcept;

private:

	static Mat3f calculateSpriteMatrix(const Sprite& sprite);

	void recalculateProjection();

	TextureHolder textureHolder;
	SpriteFactory factory;
	SpriteArray sprites;

	std::shared_ptr<SpriteRendererShaders> shaders;

	std::map<u32, SpriteBatch> batches;

	RectD viewport;          //The scene's viewport rect
	Mat4f projection;

};