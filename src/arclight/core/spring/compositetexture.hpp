/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 compositetexture.hpp
 */

#pragma once

#include "image/image.hpp"
#include "util/bitmaskenum.hpp"
#include "util/identifier.hpp"

#include <memory>
#include <unordered_map>



class TextureSet;

class CompositeTexture {

public:


	struct TextureData {

		constexpr TextureData() : width(0), height(0), arrayIndex(0) {}
		constexpr TextureData(u32 w, u32 h, const Image<Pixel::RGBA8>& img, u32 index = 0) : width(w), height(h), image(img), arrayIndex(index) {}

		u32 arrayIndex;
		u32 width;
		u32 height;
		Image<Pixel::RGBA8> image;

	};

	enum class Type {
		Array,
		Atlas,
		AtlasArray
	};

	CompositeTexture() : type(Type::Array), width(0), height(0) {}

	void bind(u32 slot);

	u32 getWidth() const noexcept;
	u32 getHeight() const noexcept;
	u32 getTextureCount() const noexcept;

	bool hasTexture(Id64 texID) const;
	const TextureData& getTextureData(Id64 texID) const;

	static CompositeTexture loadAndComposite(const TextureSet& set, Type type);

private:

	friend class TextureHandle;

	std::shared_ptr<class TextureHandle> textureHandle;

	Type type;
	u32 width;
	u32 height;

	std::unordered_map<u32, TextureData> textures;

};