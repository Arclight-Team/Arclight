/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 grayscale.hpp
 */

#pragma once

#include "image/image.hpp"
#include "common/config.hpp"
#include "common/types.hpp"


class GrayscaleFilter {

public:

	template<Pixel P>
	constexpr static void run(Image<P>& image) {

		using Format = typename Image<P>::Format;
		constexpr u32 maxValueRed = Image<P>::PixelType::getMaxRed();
		constexpr u32 maxValueGreen = Image<P>::PixelType::getMaxGreen();
		constexpr u32 maxValueBlue = Image<P>::PixelType::getMaxBlue();

		for(u32 y = 0; y < image.getHeight(); y++) {

			for(u32 x = 0; x < image.getWidth(); x++) {

				auto& pixel = image.getPixel(x, y);
				auto r = pixel.getRed();
				auto g = pixel.getGreen();
				auto b = pixel.getBlue();
				auto mixColor = (0.2126 * r) / maxValueRed + (0.7152 * g) / maxValueGreen + (0.0722 * b) / maxValueBlue;

#ifdef ARC_CFG_FILTER_EXACT
				pixel.setRGB(static_cast<u32>(Math::round(mixColor * maxValueRed)), static_cast<u32>(Math::round(mixColor * maxValueGreen)), static_cast<u32>(Math::round(mixColor * maxValueBlue)));
#else
				pixel.setRGB(static_cast<u32>(mixColor * maxValueRed), static_cast<u32>(mixColor * maxValueGreen), static_cast<u32>(mixColor * maxValueBlue));
#endif

			}

		}

	}

};