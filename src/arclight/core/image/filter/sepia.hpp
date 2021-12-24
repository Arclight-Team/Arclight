#pragma once

#include "image/image.hpp"
#include "math/matrix.hpp"
#include "arcconfig.hpp"
#include "types.hpp"



class SepiaFilter {

public:

    template<Pixel P>
    constexpr static void run(Image<P>& image) {

        using Format = typename Image<P>::Format;
        constexpr u32 maxValueRed = Image<P>::PixelType::getMaxRed();
        constexpr u32 maxValueGreen = Image<P>::PixelType::getMaxGreen();
        constexpr u32 maxValueBlue = Image<P>::PixelType::getMaxBlue();
        constexpr Mat3f sepiaMatrix = Mat3f(0.393, 0.769, 0.189, 0.349, 0.686, 0.168, 0.272, 0.534, 0.131);

        for(u32 y = 0; y < image.getHeight(); y++) {

            for(u32 x = 0; x < image.getWidth(); x++) {

                auto& pixel = image.getPixel(x, y);
                auto r = pixel.getRed() / static_cast<float>(maxValueRed);
                auto g = pixel.getGreen() / static_cast<float>(maxValueGreen);
                auto b = pixel.getBlue() / static_cast<float>(maxValueBlue);

                Vec3f c = sepiaMatrix * Vec3f(r, g, b);
                c.x = Math::min(c.x, 1.0f) * maxValueRed;
                c.y = Math::min(c.y, 1.0f) * maxValueGreen;
                c.z = Math::min(c.z, 1.0f) * maxValueBlue;

#ifdef ARC_FILTER_EXACT
                pixel.setRGB(static_cast<u32>(Math::round(c.x)), static_cast<u32>(Math::round(c.y)), static_cast<u32>(Math::round(c.z)));
#else
                pixel.setRGB(static_cast<u32>(c.x), static_cast<u32>(c.y), static_cast<u32>(c.z));
#endif

            }

        }

    }

};