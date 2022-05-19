/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 image.hpp
 */

#pragma once

#include "pixel.hpp"
#include "rawimage.hpp"
#include "math/vector.hpp"
#include "math/rectangle.hpp"
#include "types.hpp"

#include <vector>
#include <span>



enum class ImageScaling {
	Nearest,
	Bilinear
};


class ImageException : public std::runtime_error {

public:
	explicit ImageException(const std::string& msg) : std::runtime_error(msg) {}

};



template<Pixel P = Pixel::RGB8>
class Image {

public:

	using Format = PixelFormat<P>;
	using PixelType = PixelType<P>;

	constexpr static u32 PixelBytes = Format::BytesPerPixel;


	constexpr Image();
	constexpr Image(u32 width, u32 height, const PixelType& pixel = PixelType());
	constexpr Image(u32 width, u32 height, const std::span<const u8>& sourceData);

	constexpr Image(const Image& image);
	constexpr Image& operator=(const Image& image);

	constexpr Image(Image&&) = default;
	constexpr Image& operator=(Image&&) = default;

	constexpr void reset();
	constexpr void clear(const PixelType& clearPixel = PixelType());
	constexpr void setRawData(const std::span<const u8>& src, u64 startPixel = 0);

	constexpr u32 getWidth() const;
	constexpr u32 getHeight() const;
	constexpr u64 pixelCount() const;

	constexpr std::span<PixelType> getImageBuffer();
	constexpr std::span<const PixelType> getImageBuffer() const;

	constexpr void setPixel(u32 x, u32 y, const PixelType& pixel);
	constexpr PixelType& getPixel(u32 x, u32 y);
	constexpr const PixelType& getPixel(u32 x, u32 y) const;

	template<class Filter, class... Args> void applyFilter(Args&&... args);

	constexpr void resize(ImageScaling scaling, u32 w, u32 h = 0);
	constexpr void flipY();
	constexpr void copy(Image<P>& destImage, const RectUI& src, const Vec2ui& dest);
	constexpr void copy(const RectUI& src, const Vec2ui& dest);

	template<Pixel Q> Image<Q> convert() const;

	RawImage makeRaw();
	static Image fromRaw(RawImage& image, bool allowConversion = true);

	constexpr static Pixel getFormat() {
		return P;
	}

private:

	template<Pixel P> friend class Image;

	u32 width;
	u32 height;
	std::unique_ptr<PixelType[]> pixels;

};

#include "image.inl"