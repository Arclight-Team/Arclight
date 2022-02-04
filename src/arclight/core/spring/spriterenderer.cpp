/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriterenderer.cpp
 */

#include "spriterenderer.hpp"
#include "render/gle/gle.hpp"
#include "render/utility/shaderloader.hpp"
#include "render/utility/vertexhelper.hpp"
#include "debug.hpp"
#include "time/timer.hpp"
#include "time/profiler.hpp"



SpriteRenderer::SpriteRenderer() {}




void SpriteRenderer::render() {

	Profiler p;
	p.start();

	p.start();

	Vec2f pos;

	for (Sprite& sprite : sprites) {

		if (sprite.isDirty()) {

			sprite.clearDirty();

			u64 key = SpriteBatch::generateKey(0, false, false);
			//batches[key] =

		}

	}

	p.stop("Sprite B");

}



void SpriteRenderer::setViewport(const Vec2d& lowerLeft, const Vec2d& topRight) {

	viewport = RectD::fromPoints(lowerLeft, topRight);
	recalculateProjection();

}



Sprite& SpriteRenderer::createSprite(UUID id, UUID typeID) {

	Sprite& sprite = sprites.create(id);
	sprite.spriteID = typeID;

	return sprite;

}



Sprite& SpriteRenderer::getSprite(UUID id) {
	return sprites.get(id);
}



const Sprite& SpriteRenderer::getSprite(UUID id) const {
	return sprites.get(id);
}



bool SpriteRenderer::containsSprite(UUID id) const {
	return sprites.contains(id);
}



void SpriteRenderer::destroySprite(UUID id) {
	sprites.destroy(id);
}



bool SpriteRenderer::hasType(UUID id) const {
	return factory.contains(id);
}



OptionalRef<const SpriteType> SpriteRenderer::getType(UUID id) const {

	if (factory.contains(id)) {
		factory.get(id);
	}

	return {};

}



void SpriteRenderer::destroyType(UUID id) {
	factory.destroy(id);
}



u32 SpriteRenderer::activeSpriteCount() const noexcept {
	return sprites.size();
}



void SpriteRenderer::recalculateProjection() {
	projection = Mat4f::ortho(viewport.getX(), viewport.getEndX(), viewport.getY(), viewport.getEndY(), 1, -1);
}