/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 rawimage.hpp
 */

#pragma once

#include "pixel.hpp"

#include <vector>



class RawImage {

public:

	constexpr RawImage() noexcept : width(0), height(0), format(Pixel::RGB8), bufferSize(0) {}
	constexpr RawImage(u32 width, u32 height) noexcept : width(width), height(height), format(Pixel::RGB8), bufferSize(0), buffer(nullptr) {}

	template<class T>
	inline RawImage(u32 width, u32 height, T* pixels) : width(width), height(height), format(T::PixelType), bufferSize(pixels ? T::Format::BytesPerPixel * width * height : 0), buffer(Bits::toByteArray(pixels)) {}

	inline RawImage(const RawImage& image) : width(image.width), height(image.height), format(image.format), bufferSize(image.bufferSize), buffer(std::make_unique<u8[]>(image.bufferSize)) {
		std::copy_n(image.buffer.get(), bufferSize, buffer.get());
	}

	inline RawImage& operator=(const RawImage& image) {

		if (this != &image) {

			SizeT otherSize = image.getRawBuffer().size();

			if (getRawBuffer().size() != otherSize) {
				buffer = std::make_unique<u8[]>(otherSize);
			}

			width = image.getWidth();
			height = image.getHeight();
			format = image.getFormat();
			bufferSize = otherSize;

			std::copy_n(image.buffer.get(), bufferSize, buffer.get());

		}

		return *this;

	}

	RawImage(RawImage&&) = default;
	RawImage& operator=(RawImage&&) = default;

	inline bool operator==(const RawImage& image) const noexcept {
		return buffer.get() == image.buffer.get();
	}

	constexpr u32 getWidth() const noexcept {
		return width;
	}

	constexpr u32 getHeight() const noexcept {
		return height;
	}

	constexpr Pixel getFormat() const noexcept {
		return format;
	}

	constexpr bool hasData() const noexcept {
		return bufferSize;
	}

	inline std::span<const u8> getRawBuffer() const noexcept {
		return std::span{buffer.get(), bufferSize};
	}

	inline std::span<u8> release() {

		std::span<u8> data = { buffer.release(), bufferSize };

		width = 0;
		height = 0;
		bufferSize = 0;

		return data;

	}

private:

	u32 width;
	u32 height;
	Pixel format;
	SizeT bufferSize;
	std::unique_ptr<u8[]> buffer;

};