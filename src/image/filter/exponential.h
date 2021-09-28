#pragma once

#include "image/image.h"
#include "types.h"
#include "arcconfig.h"


class ExponentialFilter {

public:

    template<Pixel P>
    constexpr static void run(Image<P>& image, double exponent) {

        using Format = Image<P>::Format;
        constexpr u32 maxValueRed = Format::RedMask >> Format::RedShift;
        constexpr u32 maxValueGreen = Format::GreenMask >> Format::GreenShift;
        constexpr u32 maxValueBlue = Format::BlueMask >> Format::BlueShift;

        for(u32 y = 0; y < image.getHeight(); y++) {

            for(u32 x = 0; x < image.getWidth(); x++) {

                auto& pixel = image.getPixel(x, y);
                auto r = Math::min(Math::pow(pixel.getRed() / static_cast<float>(maxValueRed), exponent), 1.0) * maxValueRed;
                auto g = Math::min(Math::pow(pixel.getGreen() / static_cast<float>(maxValueGreen), exponent), 1.0) * maxValueGreen;
                auto b = Math::min(Math::pow(pixel.getBlue() / static_cast<float>(maxValueBlue), exponent), 1.0) * maxValueBlue;

#ifdef ARC_FILTER_EXACT
                pixel.setRGB(static_cast<u32>(Math::round(r)), static_cast<u32>(Math::round(g)), static_cast<u32>(Math::round(b)));
#else
                pixel.setRGB(static_cast<u32>(r), static_cast<u32>(g), static_cast<u32>(b));
#endif

            }

        }

    }

};