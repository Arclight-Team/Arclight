/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriterenderer.hpp
 */

#pragma once

#include "spring.hpp"
#include "compositetexture.hpp"
#include "spritearray.hpp"
#include "spritebatch.hpp"
#include "spritegroup.hpp"
#include "spritefactory.hpp"
#include "spritetypebuffer.hpp"
#include "math/rectangle.hpp"
#include "rendergroup.hpp"

#include <map>
#include <memory>
#include <unordered_map>



class TextureSet;

class SpriteRenderer {

public:

	SpriteRenderer();

	void preload();
	void render();

	void setViewport(const Vec2f& lowerLeft, const Vec2f& topRight);

	//Sprite functions
	Sprite& createSprite(Id64 id, Id32 typeID, Id32 groupID = 0);
	Sprite& getSprite(Id64 id);
	const Sprite& getSprite(Id64 id) const;
	bool containsSprite(Id64 id) const;
	void destroySprite(Id64 id);
	void destroyAllSprites();

	//Type functions
	void createType(Id32 id, Id32 textureID, const Vec2f& size);
	void createType(Id32 id, Id32 textureID, const Vec2f& size, const Vec2f& origin, const Vec2f& uvBase = Vec2f(0), const Vec2f& uvScale = Vec2f(1), SpriteOutline outline = SpriteOutline::Rectangle, const std::span<const u8>& polygon = {});
	bool hasType(Id32 id) const;
	const SpriteType& getType(Id32 id) const;

	//Texture functions
	void loadTextureSet(const TextureSet& set);

	//Group functions
	void showGroup(Id32 groupID);
	void hideGroup(Id32 groupID);
	void setGroupVisibility(Id32 groupID, bool visible);
	void toggleGroupVisibility(Id32 groupID);
	bool isGroupVisible(Id32 groupID) const;

	u32 activeSpriteCount() const noexcept;

private:

	u64 getSpriteRenderKey(const Sprite& sprite) const;
	u32 getCompositeTextureID(const Sprite& sprite) const;

	Mat2f calculateSpriteTransform(const Sprite& sprite) const;
	Vec2f calculateGlobalSpriteScale(const Sprite& sprite) const;

	void recalculateProjection();


	inline static const CTAllocationTable initialCTAllocationTable = std::vector<u32>(Spring::textureSlots, Spring::unusedCTSlot);

	SpriteFactory factory;
	SpriteArray sprites;

	std::shared_ptr<class SpriteRendererShaders> shaders;

	SpriteTypeBuffer typeBuffer;

	std::map<u32, SpriteGroup> groups;
	std::map<u64, RenderGroup> renderGroups;

	std::vector<CompositeTexture> textures;
	std::unordered_map<u32, u32> textureToCompositeID;

	RectF viewport;          //The scene's viewport rect
	Mat4f projection;

};