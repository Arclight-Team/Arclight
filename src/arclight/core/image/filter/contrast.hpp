/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 contrast.hpp
 */

#pragma once

#include "image/image.hpp"
#include "common/config.hpp"
#include "common/types.hpp"


class ContrastFilter {

public:

	template<Pixel P>
	constexpr static void run(Image<P>& image, double contrast) {

		using Format = typename Image<P>::Format;
		constexpr u32 maxValueRed = Image<P>::PixelType::getMaxRed();
		constexpr u32 maxValueGreen = Image<P>::PixelType::getMaxGreen();
		constexpr u32 maxValueBlue = Image<P>::PixelType::getMaxBlue();
		constexpr u32 halfValueRed = (maxValueRed + 1) / 2;
		constexpr u32 halfValueGreen = (maxValueGreen + 1) / 2;
		constexpr u32 halfValueBlue = (maxValueBlue + 1) / 2;

		for(u32 y = 0; y < image.getHeight(); y++) {

			for(u32 x = 0; x < image.getWidth(); x++) {

				auto& pixel = image.getPixel(x, y);
				auto r = Math::clamp(static_cast<i32>(pixel.getRed() - halfValueRed) * contrast + halfValueRed, 0, maxValueRed);
				auto g = Math::clamp(static_cast<i32>(pixel.getGreen() - halfValueGreen) * contrast + halfValueGreen, 0, maxValueGreen);
				auto b = Math::clamp(static_cast<i32>(pixel.getBlue() - halfValueBlue) * contrast + halfValueBlue, 0, maxValueBlue);

#ifdef ARC_CFG_FILTER_EXACT
				pixel.setRGB(static_cast<u32>(Math::round(r)), static_cast<u32>(Math::round(g)), static_cast<u32>(Math::round(b)));
#else
				pixel.setRGB(r, g, b);
#endif

			}

		}

	}

};