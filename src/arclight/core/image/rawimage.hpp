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

	constexpr RawImage() : RawImage(0, 0) {}
	constexpr RawImage(u32 width, u32 height) : RawImage(width, height, Pixel::RGBA8) {}
	constexpr RawImage(u32 width, u32 height, Pixel format, std::vector<u8> data = {}) : width(width), height(height), format(format), buffer(std::move(data)) {}

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
		return buffer.size();
	}

	constexpr std::span<const u8> getRawBuffer() const noexcept {
		return buffer;
	}

	constexpr void setWidth(u32 width) noexcept {
		this->width = width;
	}

	constexpr void setHeight(u32 height) noexcept {
		this->height = height;
	}

	constexpr void setData(Pixel pixelFormat, const std::vector<u8>& data) noexcept {

		format = pixelFormat;
		buffer = data;

	}

private:

	u32 width;
	u32 height;
	Pixel format;
	std::vector<u8> buffer;

};