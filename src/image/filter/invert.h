#pragma once

#include "image/image.h"
#include "types.h"



class InversionFilter {

public:

    template<Pixel P>
    constexpr static void run(Image<P>& image) {

        using Format = Image<P>::Format;
        constexpr u32 maxValueRed = Format::RedMask >> Format::RedShift;
        constexpr u32 maxValueGreen = Format::GreenMask >> Format::GreenShift;
        constexpr u32 maxValueBlue = Format::BlueMask >> Format::BlueShift;

        for(u32 y = 0; y < image.getHeight(); y++) {

            for(u32 x = 0; x < image.getWidth(); x++) {

                auto& pixel = image.getPixel(x, y);
                auto r = pixel.getRed();
                auto g = pixel.getGreen();
                auto b = pixel.getBlue();
                pixel.setRGB(maxValueRed - r, maxValueGreen - g, maxValueBlue - b);

            }

        }

    }

};