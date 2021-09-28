#pragma once

#include "pixel.h"
#include "types.h"
#include "util/assert.h"
#include <vector>
#include <span>
#include <array>



template<Pixel P = Pixel::RGBA8>
class Image {

public:

    using Format = PixelFormat<P>;
    using PixelType = PixelType<P>::Type;

    constexpr static u32 PixelBytes = Format::BytesPerPixel;


    constexpr Image() : Image(0, 0) {}
    constexpr Image(u32 width, u32 height) : width(width), height(height) {
        data.resize(width * height);
    }

    constexpr void setRawData(const std::span<u8>& src, u64 startPixel = 0) {

        SizeT pixels = data.size() / PixelBytes;
        arc_assert(startPixel + pixels <= data.size(), "Cannot copy pixel data to smaller image");

        for(SizeT i = 0; i < pixels; i++) {
            data[i + startPixel] = Format::fromBytes(data.subspan(src * PixelBytes));
        }

    }

    template<Pixel Q>
    constexpr Image<Q> convertTo() const {

        Image<Q> image(width, height);

        for(u64 i = 0; i < width * height; i++) {

            auto pixel = Format::inflate(data[i]);
            image.data[i] = PixelFormat<Q>::deflate(pixel);

        }

        return image;

    }

    constexpr u32 getWidth() const {
        return width;
    }

    constexpr u32 getHeight() const {
        return height;
    }

    constexpr std::span<PixelType> getImageBuffer() {
        return data;
    }

    constexpr std::span<const PixelType> getImageBuffer() const {
        return data;
    }

    constexpr void setPixel(u32 x, u32 y, const PixelType& pixel) {

        arc_assert(x < width && y < height, "Pixel access out of bounds");
        data[y * width + x] = pixel;

    }

    constexpr const PixelType& getPixel(u32 x, u32 y) const {
        
        arc_assert(x < width && y < height, "Pixel access out of bounds");
        return data[y * width + x];

    }

    constexpr PixelType& getPixel(u32 x, u32 y) {
        
        arc_assert(x < width && y < height, "Pixel access out of bounds");
        return data[y * width + x];

    }

    template<class Filter, class... Args>
    void applyFilter(Args&&... args) {
        Filter::run(*this, std::forward<Args>(args)...);
    }


private:

    u32 width;
    u32 height;
    std::vector<PixelType> data;

};