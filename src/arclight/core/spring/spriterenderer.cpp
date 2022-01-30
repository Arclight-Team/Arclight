/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriterenderer.cpp
 */

#include "spriterenderer.hpp"



SpriteRenderer::SpriteRenderer() {}



Sprite& SpriteRenderer::createSprite(UUID id, UUID typeID, const Mat3d& transform) {

	Sprite& sprite = sprites.create(id);
	sprite.spriteID = typeID;
	sprite.transform = transform;

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
	} else {
		return {};
	}

}



void SpriteRenderer::destroyType(UUID id) {
	factory.destroy(id);
}