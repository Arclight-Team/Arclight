/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 CompositeTexture.hpp
 */

#pragma once

#include "Image/Image.hpp"
#include "Common/Exception.hpp"

#include <memory>
#include <unordered_map>



class BadTextureCompositeException : public ArclightException {

public:
	BadTextureCompositeException() : ArclightException("Bad texture composite") {}
	virtual const char* name() const noexcept override { return "Bad Texture Composite Exception"; }

};


class TextureSet;

class CompositeTexture {

public:

	struct TextureData {

		constexpr TextureData() : x(0), y(0), width(0), height(0), arrayIndex(0), hasAlpha(false) {}
		constexpr TextureData(u32 x, u32 y, u32 w, u32 h, bool alpha, const Image<Pixel::RGBA8>& img, u32 index = 0) : x(x), y(y), width(w), height(h), hasAlpha(alpha), image(img), arrayIndex(index) {}

		u32 x;
		u32 y;
		u32 width;
		u32 height;
		u32 arrayIndex;
		Image<Pixel::RGBA8> image;
		bool hasAlpha;

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
	Vec2ui getSize() const noexcept;
	u32 getTextureCount() const noexcept;

	bool hasTexture(u32 texID) const;
	const TextureData& getTextureData(u32 texID) const;

	static CompositeTexture loadAndComposite(const TextureSet& set, Type type);

private:

	friend class TextureHandle;

	std::shared_ptr<class TextureHandle> textureHandle;

	Type type;
	u32 width;
	u32 height;

	std::unordered_map<u32, TextureData> textures;

};