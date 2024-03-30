/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Image.hpp
 */

#pragma once

#include "Pixel.hpp"
#include "Memory/AlignedAllocator.hpp"
#include "Util/Range.hpp"



class Image {

public:

	constexpr static AlignT Packed = 1;

	constexpr Image() : Image(0, 0, PixelFormat()) {}

	template<CC::PixelType Pixel>
	constexpr Image(u32 width, u32 height, Pixel pixel, AlignT rowAlignment = Packed, AlignT baseAlignment = 16)
		: Image(width, height, pixel.Format, rowAlignment, baseAlignment) {
		fill(pixel);
	}

	constexpr Image(u32 width, u32 height, PixelFormat format, AlignT rowAlignment = Packed, AlignT baseAlignment = 16)
		: width(width), height(height), format(format), stride(calculateRowStride(width, format.getBytesPerPixel(), rowAlignment)), buffer(DynamicAlignedAllocator<u8>(baseAlignment)) {

		buffer.resize(stride * height);

	}

	constexpr u32 getWidth() const {
		return width;
	}

	constexpr u32 getHeight() const {
		return height;
	}

	constexpr u64 pixelCount() const {
		return u64(width) * height;
	}

	constexpr const PixelFormat& getPixelFormat() const {
		return format;
	}

	constexpr u8* data() {
		return buffer.data();
	}

	constexpr const u8* data() const {
		return buffer.data();
	}

	constexpr void clear() {

		buffer.clear();
		width = 0;
		height = 0;
		stride = 0;

	}

	template<CC::PixelType Pixel>
	constexpr void fill(Pixel pixel) {

		SizeT offset = 0;

		for (u32 y : Range(height)) {

			for (u32 x : Range(width)) {
				std::copy(pixel.bytes().begin(), pixel.bytes().end(), buffer.begin() + offset + x * format.getBytesPerPixel());
			}

			offset += stride;

		}

	}

	template<CC::PixelType T>
	constexpr T getPixel(u32 x, u32 y) {

		SizeT i = stride + x * format.getBytesPerPixel();
		return T(std::span(buffer.data() + i, format.getBytesPerPixel()));

	}

	template<CC::PixelType T>
	constexpr void setPixel(u32 x, u32 y, const T& pixel) {

		SizeT i = stride + x * format.getBytesPerPixel();
		std::copy_n(std::begin(pixel.bytes()), format.getBytesPerPixel(), buffer.data() + i);

	}

private:

	constexpr static u32 calculateRowStride(u32 width, u32 bytesPerPixel, AlignT rowAlignment) {
		return Memory::alignUp(width * bytesPerPixel, rowAlignment);
	}

	std::vector<u8, DynamicAlignedAllocator<u8>> buffer;
	PixelFormat format;
	u32 width;
	u32 height;
	u32 stride;

};