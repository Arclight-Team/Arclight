/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriterenderer.cpp
 */

#include "spriterenderer.hpp"
#include "springshaders.hpp"
#include "compositetexture.hpp"
#include "textureset.hpp"
#include "render/gle/gle.hpp"
#include "render/utility/shaderloader.hpp"
#include "time/timer.hpp"
#include "time/profiler.hpp"
#include "debug.hpp"



struct SpriteRendererShaders {

	SpriteRendererShaders() {

		rectangleOutlineShader = ShaderLoader::fromString(SpringShader::rectangularOutlineVS, SpringShader::rectangularOutlineFS);
		uProjectionMatrix = rectangleOutlineShader.getUniform(SpringShader::rectangularOutlineUProjection);

		rectangleOutlineShader.start();

		for (u32 i = 0; i < Spring::textureSlots; i++) {

			std::string name = "textures[" + std::to_string(i) + "]";
			uTextures[i] = rectangleOutlineShader.getUniform(name.c_str());
			uTextures[i].setInt(i);

		}

	}

	~SpriteRendererShaders() {
		rectangleOutlineShader.destroy();
	}

	GLE::ShaderProgram rectangleOutlineShader;
	GLE::Uniform uProjectionMatrix;

	GLE::Uniform uTextures[Spring::textureSlots];

};



SpriteRenderer::SpriteRenderer() {}




void SpriteRenderer::render() {

	if (!shaders) {
		shaders = std::make_shared<SpriteRendererShaders>();
	}

	Profiler p;
	p.start();

	for (Sprite& sprite : sprites) {

		u8 flags = sprite.getFlags();

		if (flags) {

			u64 id = sprite.getID();
			RenderGroup& group = renderGroups[getSpriteRenderKey(sprite)];
			SpriteBatch& batch = group.getBatch();

			if (flags & Sprite::GroupDirty) {

				//The group has changed (TODO: Purge after group change)
				batch.createSprite(id, sprite.getPosition(), calculateSpriteTransform(sprite), typeBuffer.getTypeIndex(sprite.getTypeID()));
				group.addCTReference(getCompositeTextureID(sprite));

			} else if (flags & (Sprite::TranslationDirty | Sprite::TransformDirty)) {

				if (flags & Sprite::TranslationDirty) {
					batch.setSpriteTranslation(id, sprite.getPosition());
				}

				if (flags & Sprite::TransformDirty) {
					batch.setSpriteTransform(id, calculateSpriteTransform(sprite));
				}

			} else if (flags & Sprite::TypeDirty) {

				u32 oldTypeIndex = batch.getSpriteTypeIndex(id);
				u32 newTypeIndex = typeBuffer.getTypeIndex(sprite.getTypeID());

				if (oldTypeIndex != newTypeIndex) {

					batch.setSpriteTypeIndex(id, newTypeIndex);
					group.removeCTReference(oldTypeIndex);
					group.addCTReference(newTypeIndex);

				}

			}

			sprite.clearFlags();

		}

	}

	GLE::clear(GLE::Color);


	shaders->rectangleOutlineShader.start();
	shaders->uProjectionMatrix.setMat4(projection);

	typeBuffer.update();
	typeBuffer.bind();

	const CTAllocationTable* prevCTAT = &initialCTAllocationTable;

	for (auto& [key, group] : renderGroups) {

		group.prepareCTTable(*prevCTAT);
		prevCTAT = &group.getCTAllocationTable();

		for (const auto& [ctID, slot] : group.getCTBindings()) {
			textures[ctID].bind(slot);
		}

		group.syncData();
		group.render();

	}

	//p.stop("Sprite B");

}



void SpriteRenderer::setViewport(const Vec2f& lowerLeft, const Vec2f& topRight) {

	viewport = RectF::fromPoints(lowerLeft, topRight);
	recalculateProjection();

}



Sprite& SpriteRenderer::createSprite(Id64 id, Id32 typeID, Id32 groupID) {

	Sprite& sprite = sprites.create(id);
	sprite.id = id;
	sprite.typeID = typeID;
	sprite.groupID = groupID;

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

	RenderGroup& group = renderGroups[getSpriteRenderKey(sprite)];
	group.removeCTReference(getCompositeTextureID(sprite));

	sprites.destroy(id);

}



void SpriteRenderer::createType(Id32 id, Id32 textureID, const Vec2f& size) {
	createType(id, textureID, size, Vec2f(0), SpriteOutline());
}



void SpriteRenderer::createType(Id32 id, Id32 textureID, const Vec2f& size, const Vec2f& origin, const SpriteOutline& outline) {

	const SpriteType& type = factory.create(id, textureID, size, origin, outline);

	u32 ctID = textureToCompositeID[textureID];
	u32 texID = textures[ctID].getTextureData(textureID).arrayIndex;

	typeBuffer.addType(id, type, ctID, texID);

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



void SpriteRenderer::showGroup(Id32 groupID) {
	groups[groupID].show();
}



void SpriteRenderer::hideGroup(Id32 groupID) {
	groups[groupID].hide();
}



void SpriteRenderer::setGroupVisibility(Id32 groupID, bool visible) {
	groups[groupID].setVisibility(visible);
}



void SpriteRenderer::toggleGroupVisibility(Id32 groupID) {
	groups[groupID].toggleVisibility();
}



bool SpriteRenderer::isGroupVisible(Id32 groupID) const {

	auto it = groups.find(groupID);

	if (it != groups.end()) {
		return it->second.isVisible();
	}

	return true;

}



u32 SpriteRenderer::activeSpriteCount() const noexcept {
	return sprites.size();
}



u64 SpriteRenderer::getSpriteRenderKey(const Sprite& sprite) const {
	return sprite.getGroupID();
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