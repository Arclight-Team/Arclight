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
#include "image/bmp.hpp"
#include "debug.hpp"
#include "filesystem/file.hpp"
#include "stream/fileinputstream.hpp"



struct SpriteRendererShaders {

	SpriteRendererShaders() {

		rectangleOutlineShader = ShaderLoader::fromString(SpringShader::rectangularOutlineVS, SpringShader::rectangularOutlineFS);
		uProjectionMatrix = rectangleOutlineShader.getUniform(SpringShader::rectangularOutlineUProjection);
		uSampleTexture = rectangleOutlineShader.getUniform("sampleTexture");

		File file("@/cursor.bmp", File::Binary | File::In);
		file.open();
		FileInputStream stream(file);
		Image img = BMP::loadBitmap<Pixel::RGBA8>(stream);

		sampleTexture.create();
		sampleTexture.bind();
		sampleTexture.setData(img.getWidth(), img.getHeight(), GLE::ImageFormat::RGBA8, GLE::TextureSourceFormat::RGBA, GLE::TextureSourceType::UByte, img.getImageBuffer().data());
		sampleTexture.setMinFilter(GLE::TextureFilter::None);
		sampleTexture.setMagFilter(GLE::TextureFilter::None);
		sampleTexture.generateMipmaps();

	}

	~SpriteRendererShaders() {
		rectangleOutlineShader.destroy();
		sampleTexture.destroy();
	}

	GLE::ShaderProgram rectangleOutlineShader;
	GLE::Uniform uProjectionMatrix;

	GLE::Texture2D sampleTexture;
	GLE::Uniform uSampleTexture;

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
				batch.createSprite(id, sprite.getPosition(), calculateSpriteTransform(sprite));

			} else if (flags & (Sprite::TranslationDirty | Sprite::TransformDirty)) {

				if (flags & Sprite::TranslationDirty) {
					batch.setSpriteTranslation(id, sprite.getPosition());
				}

				if (flags & Sprite::TransformDirty) {
					batch.setSpriteTransform(id, calculateSpriteTransform(sprite));
				}

			}

			sprite.clearFlags();

		}

	}

	GLE::clear(GLE::Color);

	shaders->rectangleOutlineShader.start();
	shaders->uProjectionMatrix.setMat4(projection);

	shaders->sampleTexture.activate(0);
	shaders->uSampleTexture.setInt(0);

	for (auto& [key, batch] : batches) {

		batch.synchronize();

		if (groups[key].isVisible()) {
			batch.render();
		}

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



void SpriteRenderer::createType(Id32 id, Id32 textureID, const Vec2f& size) {
	createType(id, textureID, size, Vec2f(0), SpriteOutline());
}



void SpriteRenderer::createType(Id32 id, Id32 textureID, const Vec2f& size, const Vec2f& origin, const SpriteOutline& outline) {
	factory.create(id, textureID, size, origin, outline);
}



bool SpriteRenderer::hasType(Id32 id) const {
	return factory.contains(id);
}



const SpriteType& SpriteRenderer::getType(Id32 id) const {
	return factory.get(id);
}



void SpriteRenderer::destroyType(Id32 id) {
	factory.destroy(id);
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