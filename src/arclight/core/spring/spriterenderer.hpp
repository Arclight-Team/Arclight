/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriterenderer.hpp
 */

#pragma once

#include "compositetexture.hpp"
#include "spring.hpp"
#include "springshader.hpp"
#include "spritearray.hpp"
#include "spritebatch.hpp"
#include "spritegroup.hpp"
#include "spritefactory.hpp"
#include "spritetypebuffer.hpp"

#include "math/rectangle.hpp"
#include "shaderpool.hpp"

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
	Sprite& createSprite(Id64 id, Id32 typeID, u32 groupID = 0);
	Sprite& getSprite(Id64 id);
	const Sprite& getSprite(Id64 id) const;
	bool containsSprite(Id64 id) const;
	void destroySprite(Id64 id);
	void destroyAllSprites();

	//Type functions
	void createType(Id32 id, Id32 textureID, const Vec2f& size);
	void createType(Id32 id, Id32 textureID, const Vec2f& size, const Vec2f& origin, const Vec2f& uvBase = Vec2f(0), const Vec2f& uvScale = Vec2f(1), SpriteOutline outline = SpriteOutline::Rectangle, const std::span<const u8>& polygon = {});
	void createType(Id32 id, u32 shaderID, Id32 textureID, const Vec2f& size);
	void createType(Id32 id, u32 shaderID, Id32 textureID, const Vec2f& size, const Vec2f& origin, const Vec2f& uvBase = Vec2f(0), const Vec2f& uvScale = Vec2f(1), SpriteOutline outline = SpriteOutline::Rectangle, const std::span<const u8>& polygon = {});
	bool hasType(Id32 id) const;
	const SpriteType& getType(Id32 id) const;

	//Texture functions
	void loadTextureSet(const TextureSet& set);

	//Group functions
	void showGroup(u32 shaderID, u32 groupID);
	void hideGroup(u32 shaderID, u32 groupID);
	void setGroupVisibility(u32 shaderID, u32 groupID, bool visible);
	void toggleGroupVisibility(u32 shaderID, u32 groupID);
	bool isGroupVisible(u32 shaderID, u32 groupID) const;

	void setGroupCount(u32 shaderID, u32 count);

	//Shader functions
	void registerShader(const SpringShader& shader);
	void unregisterShader(u32 shaderID);
	bool isShaderRegistered(u32 shaderID);
	void enableShader(u32 shaderID);
	void disableShader(u32 shaderID);
	bool isShaderEnabled(u32 shaderID) const;

	u32 activeSpriteCount() const noexcept;

	ShaderPool& getShaderPool();

private:

	void loadStandardShaders();

	SpriteGroup& getGroup(u32 shaderID, u32 groupID);
	const SpriteGroup& getGroup(u32 shaderID, u32 groupID) const;

	SpringShader& getShader(u32 shaderID);
	const SpringShader& getShader(u32 shaderID) const;

	u32 getShaderID(const Sprite& sprite) const;
	u32 getCompositeTextureID(const Sprite& sprite) const;

	Mat2f calculateSpriteTransform(const Sprite& sprite) const;
	Vec2f calculateGlobalSpriteScale(const Sprite& sprite) const;

	void recalculateProjection();


	inline static const CTAllocationTable initialCTAllocationTable = std::vector<u32>(Spring::textureSlots, Spring::unusedCTSlot);

	SpriteFactory factory;
	SpriteArray sprites;

	SpriteTypeBuffer typeBuffer;

	ShaderPool shaderPool;

	std::unordered_map<u32, std::vector<SpriteGroup>> groups;   //ShaderID -> Group
	std::map<u32, SpringShader> shaders;                        //ShaderID -> Shader

	std::vector<CompositeTexture> textures;
	std::unordered_map<u32, u32> textureToCompositeID;          //TexID -> CTID

	RectF viewport;          //The scene's viewport rect
	Mat4f projection;

};