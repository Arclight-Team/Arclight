/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SpriteRenderer.hpp
 */

#pragma once

#include "CompositeTexture.hpp"
#include "Spring.hpp"
#include "SpriteShader.hpp"
#include "Sprite.hpp"
#include "SpriteBatch.hpp"
#include "SpriteGroup.hpp"
#include "SpriteFactory.hpp"
#include "SpriteTypeBuffer.hpp"
#include "ShaderPool.hpp"
#include "SharedBuffer.hpp"

#include "Math/Shapes/Rectangle.hpp"
#include "STDExt/ArrayMap.hpp"

#include <map>
#include <memory>
#include <unordered_map>



class TextureSet;

class SpriteRenderer {

public:

	SpriteRenderer() = default;

	void initialize();
	void render();

	void setViewport(const Vec2f& lowerLeft, const Vec2f& topRight);

	//Sprite functions
	Sprite& createSprite(Id64 id, Id32 typeID, u32 groupID = 0, u32 shaderID = Spring::baseShaderID);
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
	void showGroup(u32 shaderID, u32 groupID);
	void hideGroup(u32 shaderID, u32 groupID);
	void setGroupVisibility(u32 shaderID, u32 groupID, bool visible);
	void toggleGroupVisibility(u32 shaderID, u32 groupID);
	bool isGroupVisible(u32 shaderID, u32 groupID) const;

	void setGroupCount(u32 shaderID, u32 count);

	//Shader functions
	void registerShader(u32 shaderID, const SpringShader& shader);
	void unregisterShader(u32 shaderID);
	bool isShaderRegistered(u32 shaderID);
	void enableShader(u32 shaderID);
	void disableShader(u32 shaderID);
	bool isShaderEnabled(u32 shaderID) const;

	//Visitor
	template<class Func>
	void spriteExecute(Func&& func) requires CC::Invocable<Func, Sprite&> {

		for (Sprite& sprite : sprites) {
			func(sprite);
		}

	}

	template<class Func>
	void groupExecute(u32 shaderID, u32 groupID, Func&& func) requires CC::Invocable<Func, Sprite&> {

		for (Sprite& sprite : sprites) {

			if (sprite.prevShaderID == shaderID && sprite.prevGroupID == groupID) {
				func(sprite);
			}

		}

	}

	u32 activeSpriteCount() const noexcept;

	ShaderPool& getShaderPool();

private:

	SpriteGroup& getGroup(u32 shaderID, u32 groupID);
	const SpriteGroup& getGroup(u32 shaderID, u32 groupID) const;

	SpringShader& getShader(u32 shaderID);
	const SpringShader& getShader(u32 shaderID) const;

	u32 getCompositeTextureID(const Sprite& sprite) const;

	Mat2f calculateSpriteTransform(const Sprite& sprite) const;
	Vec2f calculateGlobalSpriteScale(const Sprite& sprite) const;

	void recalculateProjection();


	inline static const CTAllocationTable initialCTAllocationTable = std::vector<u32>(Spring::textureSlots, Spring::unusedCTSlot);

	SpriteFactory factory;
	ArrayMap<u64, Sprite> sprites;

	SpriteTypeBuffer typeBuffer;
	SharedBuffer sharedBuffer;

	ShaderPool shaderPool;

	std::unordered_map<u32, std::vector<SpriteGroup>> groups;   //ShaderID -> Group
	std::map<u32, SpringShader> shaders;                        //ShaderID -> Shader

	std::vector<CompositeTexture> textures;
	std::unordered_map<u32, u32> textureToCompositeID;          //TexID -> CTID

	RectF viewport;          //The scene's viewport rect
	Mat4f projection;

};