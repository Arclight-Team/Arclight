/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriterenderer.cpp
 */

#include "spriterenderer.hpp"
#include "compositetexture.hpp"
#include "textureset.hpp"
#include "render/gle/gle.hpp"
#include "render/utility/shaderloader.hpp"
#include "time/profiler.hpp"
#include "debug.hpp"



void SpriteRenderer::initialize() {

	typeBuffer.create();
	sharedBuffer.create();

	ShaderStage stage(shaderPool.get("@/shaders/spring/spring_rect_opaque.vs", "@/shaders/spring/spring_rect_opaque.fs"), ShaderStage::PipelineFlags::Textures | ShaderStage::PipelineFlags::Culling);
	SpringShader shader(stage, Spring::baseShaderID);

	registerShader(shader);

}



void SpriteRenderer::render() {

	Profiler p;
	p.start();

	sharedBuffer.update();
	sharedBuffer.bind();

	for (Sprite& sprite : sprites) {

		u8 flags = sprite.getFlags();

		if (flags) {

			u64 id = sprite.getID();

			if (flags & Sprite::Created) {

				SpriteGroup& group = getGroup(sprite.shaderID, sprite.groupID);

				group.getBatch().createSprite(id, sprite.typeID, sprite.getPosition(), calculateSpriteTransform(sprite));
				group.addCTReference(getCompositeTextureID(sprite));

			} else if (flags & (Sprite::ShaderDirty | Sprite::GroupDirty) && (sprite.prevGroupID != sprite.groupID || sprite.prevShaderID != sprite.shaderID)) {

				u32 newGroupSize = groups[sprite.shaderID].size();
				u32 newGroupID = newGroupSize - 1;

				if (flags & Sprite::GroupDirty && sprite.groupID <= newGroupID) {
					newGroupID = sprite.groupID;
				}

				SpriteGroup& oldGroup = getGroup(sprite.prevShaderID, sprite.prevGroupID);
				SpriteGroup& newGroup = getGroup(sprite.shaderID, newGroupID);

				oldGroup.getBatch().purgeSprite(id);
				newGroup.getBatch().createSprite(id, sprite.typeID, sprite.position, calculateSpriteTransform(sprite));

				u32 oldCTID = textureToCompositeID[getType(sprite.prevTypeID).textureID];
				u32 newCTID = textureToCompositeID[getType(sprite.typeID).textureID];

				oldGroup.removeCTReference(oldCTID);
				newGroup.addCTReference(newCTID);

				sprite.prevGroupID = sprite.groupID;
				sprite.prevShaderID = sprite.shaderID;

			} else {

				if (flags & Sprite::TypeDirty) {

					const SpriteType& oldType = getType(sprite.prevTypeID);
					const SpriteType& newType = getType(sprite.typeID);

					u32 oldCTID = textureToCompositeID[oldType.textureID];
					u32 newCTID = textureToCompositeID[newType.textureID];

					SpriteGroup& group = getGroup(sprite.shaderID, sprite.groupID);
					group.getBatch().setSpriteTypeIndex(id, sprite.typeID);

					if (oldCTID != newCTID) {

						group.removeCTReference(oldCTID);
						group.addCTReference(newCTID);

					}

					sprite.prevTypeID = sprite.typeID;

				}

				if (flags & (Sprite::TranslationDirty | Sprite::TransformDirty)) {

					SpriteBatch& batch = getGroup(sprite.shaderID, sprite.groupID).getBatch();

					if (flags & Sprite::TranslationDirty) {
						batch.setSpriteTranslation(id, sprite.getPosition());
					}

					if (flags & Sprite::TransformDirty) {
						batch.setSpriteTransform(id, calculateSpriteTransform(sprite));
					}

				}

			}

			sprite.clearFlags();

		}

	}

	GLE::clear(GLE::Color);

	typeBuffer.update();
	typeBuffer.bind();

	const CTAllocationTable* prevCTAT = &initialCTAllocationTable;

	for (auto& [siid, shader] : shaders) {

		std::vector<SpriteGroup>& renderGroups = groups[siid];

		for (SpriteGroup& group : renderGroups) {
			group.syncData();
		}

		if (shader.isEnabled()) {

			for (u32 i = 0; i < shader.getStageCount(); i++) {

				if (!shader.preRender(i)) {
					break;
				}

				for (SpriteGroup& group : renderGroups) {

					if (shader.hasStageFlag(i, ShaderStage::PipelineFlags::Textures)) {

						group.prepareCTTable(*prevCTAT, true);
						prevCTAT = &group.getCTAllocationTable();

						for (const auto& [ctID, slot] : group.getCTBindings()) {
							textures[ctID].bind(slot);
						}

					}

					if (group.isVisible()) {

						if (shader.hasStageFlag(i, ShaderStage::PipelineFlags::Depth)) {
							GLE::enableDepthTests();
						} else {
							GLE::disableBlending();
						}

						if (shader.hasStageFlag(i, ShaderStage::PipelineFlags::Blending)) {
							GLE::enableBlending();
						} else {
							GLE::disableBlending();
						}

						if (shader.hasStageFlag(i, ShaderStage::PipelineFlags::Culling)) {
							GLE::enableCulling();
						} else {
							GLE::disableCulling();
						}

						group.render();

					}

				}

				shader.postRender(i);

			}

		}

	}

	//p.stop("Sprite B");

}



void SpriteRenderer::setViewport(const Vec2f& lowerLeft, const Vec2f& topRight) {

	viewport = RectF::fromPoints(lowerLeft, topRight);
	recalculateProjection();

	sharedBuffer.setProjection(projection);

}



Sprite& SpriteRenderer::createSprite(Id64 id, Id32 typeID, u32 groupID, u32 shaderID) {

	Sprite& sprite = sprites.create(id);
	sprite.id = id;
	sprite.typeID = typeID;
	sprite.groupID = groupID;
	sprite.shaderID = shaderID;
	sprite.prevTypeID = typeID;
	sprite.prevGroupID = groupID;
	sprite.prevShaderID = shaderID;

	return sprite;

}



Sprite& SpriteRenderer::getSprite(Id64 id) {
	return sprites.get(id);
}



const Sprite& SpriteRenderer::getSprite(Id64 id) const {
	return sprites.get(id);
}



bool SpriteRenderer::containsSprite(Id64 id) const {
	return sprites.contains(id);
}



void SpriteRenderer::destroySprite(Id64 id) {

	if (!containsSprite(id)) {
		return;
	}

	const Sprite& sprite = getSprite(id);
	const SpriteType& type = getType(sprite.prevTypeID);

	u32 ctID = textureToCompositeID[type.textureID];

	SpriteGroup& group = getGroup(sprite.prevShaderID, sprite.prevGroupID);
	group.removeCTReference(ctID);
	group.getBatch().purgeSprite(id);

	sprites.destroy(id);

}



void SpriteRenderer::destroyAllSprites() {

	sprites.clear();

	for (auto& [shaderID, groupList] : groups) {
		groupList = { SpriteGroup() };
	}

}



void SpriteRenderer::createType(Id32 id, Id32 textureID, const Vec2f& size) {
	createType(id, textureID, size, Vec2f(0));
}



void SpriteRenderer::createType(Id32 id, Id32 textureID, const Vec2f& size, const Vec2f& origin, const Vec2f& uvBase, const Vec2f& uvScale, SpriteOutline outline, const std::span<const u8>& polygon) {

	u32 ctID = textureToCompositeID[textureID];

	const CompositeTexture& ct = textures[ctID];
	CompositeTexture::TextureData texData = ct.getTextureData(textureID);

	Vec2f newUvBase = uvBase + Vec2f(texData.x, texData.y) / ct.getSize();
	Vec2f newUvScale = uvScale * Vec2f(texData.width, texData.height) / ct.getSize();

	const SpriteType& type = factory.create(id, textureID, size, origin, newUvBase, newUvScale, outline, polygon);
	typeBuffer.addType(id, type, ctID, texData.arrayIndex);

}



bool SpriteRenderer::hasType(Id32 id) const {
	return factory.contains(id);
}



const SpriteType& SpriteRenderer::getType(Id32 id) const {
	return factory.get(id);
}



void SpriteRenderer::loadTextureSet(const TextureSet& set) {

	u32 ctID = textures.size();

	CompositeTexture texture = CompositeTexture::loadAndComposite(set, CompositeTexture::Type::Array);
	textures.emplace_back(texture);

	for (const auto& [id, path] : set.getTexturePaths()) {
		textureToCompositeID.try_emplace(id, ctID);
	}

}



void SpriteRenderer::showGroup(u32 shaderID, u32 groupID) {
	getGroup(shaderID, groupID).show();
}



void SpriteRenderer::hideGroup(u32 shaderID, u32 groupID) {
	getGroup(shaderID, groupID).hide();
}



void SpriteRenderer::setGroupVisibility(u32 shaderID, u32 groupID, bool visible) {
	getGroup(shaderID, groupID).setVisibility(visible);
}



void SpriteRenderer::toggleGroupVisibility(u32 shaderID, u32 groupID) {
	getGroup(shaderID, groupID).toggleVisibility();
}



bool SpriteRenderer::isGroupVisible(u32 shaderID, u32 groupID) const {
	return getGroup(shaderID, groupID).isVisible();
}



void SpriteRenderer::setGroupCount(u32 shaderID, u32 count) {
	groups[shaderID].resize(count);
}



void SpriteRenderer::registerShader(const SpringShader& shader) {

	u32 shaderID = shader.getInvocationID();

	if (shaders.contains(shaderID)) {
		Log::warn("Spring", "Failed to register shader %d: ID already in use", shaderID);
		return;
	}

	shaders.try_emplace(shaderID, shader);
	groups.emplace(shaderID, std::vector<SpriteGroup> { SpriteGroup() });

}



void SpriteRenderer::unregisterShader(u32 shaderID) {

	auto shIt = shaders.find(shaderID);

	if (shIt == shaders.end()) {
		Log::warn("Spring", "Failed to unregister shader %d: ID not found", shaderID);
		return;
	}

	shaders.erase(shIt);
	groups.erase(shaderID);

}



bool SpriteRenderer::isShaderRegistered(u32 shaderID) {
	return shaders.contains(shaderID);
}



void SpriteRenderer::enableShader(u32 shaderID) {
	getShader(shaderID).enable();
}



void SpriteRenderer::disableShader(u32 shaderID) {
	getShader(shaderID).disable();
}



bool SpriteRenderer::isShaderEnabled(u32 shaderID) const {
	return getShader(shaderID).isEnabled();
}



u32 SpriteRenderer::activeSpriteCount() const noexcept {
	return sprites.size();
}



ShaderPool& SpriteRenderer::getShaderPool() {
	return shaderPool;
}



SpriteGroup& SpriteRenderer::getGroup(u32 shaderID, u32 groupID) {
	return groups[shaderID][groupID];
}



const SpriteGroup& SpriteRenderer::getGroup(u32 shaderID, u32 groupID) const {
	return groups.find(shaderID)->second[groupID];
}



SpringShader& SpriteRenderer::getShader(u32 shaderID) {
	return shaders[shaderID];
}



const SpringShader& SpriteRenderer::getShader(u32 shaderID) const {
	return shaders.find(shaderID)->second;
}



u32 SpriteRenderer::getCompositeTextureID(const Sprite& sprite) const {
	return textureToCompositeID.find(getType(sprite.getTypeID()).textureID)->second;
}



Mat2f SpriteRenderer::calculateSpriteTransform(const Sprite& sprite) const {

	Vec2f globalScale = calculateGlobalSpriteScale(sprite);
	return Mat3f::fromRotation(sprite.getRotation()).shear(sprite.getShearX(), sprite.getShearY()).scale(globalScale.x, globalScale.y).toMat2();

}



Vec2f SpriteRenderer::calculateGlobalSpriteScale(const Sprite& sprite) const {
	return sprite.getScale() * getType(sprite.getTypeID()).size;
}



void SpriteRenderer::recalculateProjection() {
	projection = Mat4f::ortho(viewport.getX(), viewport.getEndX(), viewport.getY(), viewport.getEndY(), -1, 1);
}