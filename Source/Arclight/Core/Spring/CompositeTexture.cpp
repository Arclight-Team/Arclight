/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 CompositeTexture.cpp
 */

#include "CompositeTexture.hpp"
#include "TextureSet.hpp"
#include "Filesystem/File.hpp"
#include "Filesystem/Path.hpp"
#include "Render/GLE/GLE.hpp"
#include "Image/ImageIO.hpp"



struct TextureHandle {

	explicit TextureHandle(CompositeTexture& compositeTexture) {

		texture.create();
		texture.bind();
		texture.setData(compositeTexture.getWidth(), compositeTexture.getHeight(), compositeTexture.getTextureCount(), GLE::ImageFormat::RGBA8, GLE::TextureSourceFormat::RGBA, GLE::TextureSourceType::UByte, nullptr);

		for (u32 layerID = 0; auto& [id, texData] : compositeTexture.textures) {

			texData.arrayIndex = layerID;
			texture.update(0, 0, texData.width, texData.height, layerID++, GLE::TextureSourceFormat::RGBA, GLE::TextureSourceType::UByte, texData.image.getImageBuffer().data());
			texData.image.reset();

		}

		texture.setMinFilter(GLE::TextureFilter::None);
		texture.setMagFilter(GLE::TextureFilter::None);
		texture.setWrapU(GLE::TextureWrap::Clamp);
		texture.setWrapV(GLE::TextureWrap::Clamp);
		texture.generateMipmaps();

	}

	~TextureHandle() {
		texture.destroy();
	}

	GLE::ArrayTexture2D texture;

};



void CompositeTexture::bind(u32 slot) {

	arc_assert(textureHandle, "Composite texture handle cannot be null");

	GLE::ArrayTexture2D& arrayTexture = textureHandle->texture;
	arrayTexture.activate(slot);

}



u32 CompositeTexture::getWidth() const noexcept {
	return width;
}



u32 CompositeTexture::getHeight() const noexcept {
	return height;
}



Vec2ui CompositeTexture::getSize() const noexcept {
	return { width, height };
}



u32 CompositeTexture::getTextureCount() const noexcept {
	return textures.size();
}



bool CompositeTexture::hasTexture(Id32 texID) const {
	return textures.contains(texID);
}



const CompositeTexture::TextureData& CompositeTexture::getTextureData(Id32 texID) const {
	return textures.find(texID)->second;
}



CompositeTexture CompositeTexture::loadAndComposite(const TextureSet& set, Type type) {

	try {

		const std::unordered_map<u32, TextureLoadData>& loadData = set.getTexturePaths();
		std::unordered_map<u32, TextureData> textures;

		u32 maxWidth = 0;
		u32 maxHeight = 0;

		for(const auto& [id, data] : loadData) {

			Image image = ImageIO::load<Pixel::RGBA8>(data.path);

			maxWidth = Math::max(image.getWidth(), maxWidth);
			maxHeight = Math::max(image.getHeight(), maxHeight);

			textures.emplace(id, TextureData(0, 0, image.getWidth(), image.getHeight(), data.hasAlpha, std::move(image)));

		}

		CompositeTexture texture;
		texture.width = maxWidth;
		texture.height = maxHeight;
		texture.type = type;
		texture.textures = std::move(textures);
		texture.textureHandle = std::make_shared<TextureHandle>(texture);

		return texture;

	} catch (const std::exception&) {

		throw BadTextureCompositeException();

	}

}