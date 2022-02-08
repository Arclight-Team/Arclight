/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriterenderer.cpp
 */

#include "spriterenderer.hpp"
#include "springshaders.hpp"
#include "render/gle/gle.hpp"
#include "render/utility/shaderloader.hpp"
#include "time/timer.hpp"
#include "time/profiler.hpp"
#include "debug.hpp"



struct SpriteRendererShaders {

	SpriteRendererShaders() {
		rectangleOutlineShader = ShaderLoader::fromString(SpringShader::rectangularOutlineVS, SpringShader::rectangularOutlineFS);
		uProjectionMatrix = rectangleOutlineShader.getUniform(SpringShader::rectangularOutlineUProjection);
	}

	~SpriteRendererShaders() {
		rectangleOutlineShader.destroy();
	}

	GLE::ShaderProgram rectangleOutlineShader;
	GLE::Uniform uProjectionMatrix;

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
			SpriteBatch& batch = batches[sprite.getGroupID()];

			if (flags & Sprite::GroupDirty) {

				//The group has changed (TODO: Purge after group change)
				batch.createSprite(id, calculateSpriteMatrix(sprite), sprite.getPosition());

			} else if (flags & Sprite::TransformDirty) {

				if (flags & Sprite::MatrixDirty) {

					//Full transform update
					batch.setSpriteTransform(id, calculateSpriteMatrix(sprite), sprite.getPosition());

				} else {

					//Translation update
					batch.setSpriteTranslation(id, sprite.getPosition());

				}

			}

			sprite.clearFlags();

		}

	}

	GLE::clear(GLE::Color);

	shaders->rectangleOutlineShader.start();
	shaders->uProjectionMatrix.setMat4(projection);

	for (auto& [key, batch] : batches) {

		batch.synchronize();
		batch.render();

	}

	p.stop("Sprite B");

}



void SpriteRenderer::setViewport(const Vec2d& lowerLeft, const Vec2d& topRight) {

	viewport = RectD::fromPoints(lowerLeft, topRight);
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
	sprites.destroy(id);
}



bool SpriteRenderer::hasType(Id64 id) const {
	return factory.contains(id);
}



OptionalRef<const SpriteType> SpriteRenderer::getType(Id64 id) const {

	if (factory.contains(id)) {
		factory.get(id);
	}

	return {};

}



void SpriteRenderer::destroyType(Id64 id) {
	factory.destroy(id);
}



u32 SpriteRenderer::activeSpriteCount() const noexcept {
	return sprites.size();
}



Mat2f SpriteRenderer::calculateSpriteMatrix(const Sprite& sprite) {
	return Mat3f::fromShear(sprite.getShearX(), sprite.getShearY()).scale(sprite.getScaleX(), sprite.getScaleY()).rotate(sprite.getRotation()).toMat2();
}



void SpriteRenderer::recalculateProjection() {
	projection = Mat4f::ortho(viewport.getX(), viewport.getEndX(), viewport.getY(), viewport.getEndY(), -1, 1);
}