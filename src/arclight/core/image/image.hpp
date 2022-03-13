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
#include "decode/decoder.hpp"
#include "filesystem/file.hpp"
#include "math/math.hpp"
#include "math/vector.hpp"
#include "math/rectangle.hpp"
#include "util/assert.hpp"
#include "types.hpp"
#include "util/bool.hpp"

#include <vector>
#include <span>
#include <array>



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


	constexpr void reset();
	constexpr void clear(const PixelType& clearPixel = PixelType());
	constexpr void setRawData(const std::span<const u8>& src, u64 startPixel = 0);

	constexpr u32 getWidth() const;
	constexpr u32 getHeight() const;

	constexpr std::span<PixelType> getImageBuffer();
	constexpr std::span<const PixelType> getImageBuffer() const;

	constexpr void setPixel(u32 x, u32 y, const PixelType& pixel);
	constexpr PixelType& getPixel(u32 x, u32 y);
	constexpr const PixelType& getPixel(u32 x, u32 y) const;

	template<class Filter, class... Args>void applyFilter(Args&&... args);

	constexpr void resize(ImageScaling scaling, u32 w, u32 h = 0);
	constexpr void flipY();
	constexpr void copy(Image<P>& destImage, const RectUI& src, const Vec2ui& dest);
	constexpr void copy(const RectUI& src, const Vec2ui& dest);

	template<Pixel Q> Image<Q> convert() const;

	static RawImage makeRaw(const Image& image);
	static Image fromRaw(const RawImage& image);

	template<ImageDecoder Decoder, class... Args> static Image load(const Path& path, Args&&... args);
	template<ImageDecoder Decoder, class... Args> static Image load(const std::span<const u8>& bytes, Args&&... args);
	template<ImageDecoder Decoder, class... Args> static Decoder decode(const Path& path, Args&&... args);
	template<ImageDecoder Decoder, class... Args> static Decoder decode(const std::span<const u8>& bytes, Args&&... args);

private:

	static std::vector<u8> loadFile(const Path& path);

	u32 width;
	u32 height;
	std::vector<PixelType> data;

};

#include "image.inl"