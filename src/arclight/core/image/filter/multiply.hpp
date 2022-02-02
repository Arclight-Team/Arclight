/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 multiply.hpp
 */

#pragma once

#include "image/image.hpp"
#include "types.hpp"


class MultiplicationFilter {

public:

	enum FilterChannel {
		Red = 1,
		Green = 2,
		Blue = 4,
		Alpha = 8
	};

	template<Pixel P>
	constexpr static void run(Image<P>& image, u32 channel, double amount) {

		using Format = typename Image<P>::Format;
		constexpr u32 maxValueRed = Image<P>::PixelType::getMaxRed();
		constexpr u32 maxValueGreen = Image<P>::PixelType::getMaxGreen();
		constexpr u32 maxValueBlue = Image<P>::PixelType::getMaxBlue();
		constexpr u32 maxValueAlpha = Image<P>::PixelType::getMaxAlpha();

		amount = Math::max(amount, 0);

		for(u32 y = 0; y < image.getHeight(); y++) {

			for(u32 x = 0; x < image.getWidth(); x++) {

				auto& pixel = image.getPixel(x, y);
				auto r = pixel.getRed();
				auto g = pixel.getGreen();
				auto b = pixel.getBlue();
				auto a = pixel.getAlpha();

				if (channel & Red) {
					r = Math::min(r * amount, maxValueRed);
				}
				if (channel & Green) {
					g = Math::min(g * amount, maxValueGreen);
				}
				if (channel & Blue) {
					b = Math::min(b * amount, maxValueBlue);
				}
				if (channel & Alpha) {
					a = Math::min(a * amount, maxValueAlpha);
				}

				pixel.setRGBA(r, g, b, a);

			}

		}

	}

};