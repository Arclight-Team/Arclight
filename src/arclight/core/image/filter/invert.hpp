#pragma once

#include "image/image.hpp"
#include "types.hpp"



class InversionFilter {

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
                pixel.setRGB(maxValueRed - r, maxValueGreen - g, maxValueBlue - b);

            }

        }

    }

};