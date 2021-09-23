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

    constexpr static u32 PixelBytes = PixelFormat<P>::BytesPerPixel;


    constexpr Image() : Image(0, 0, {}) {}
    constexpr Image(u32 width, u32 height, const std::span<u8>& data = {}) : width(width), height(height) {
        setRawData(data);
    }

    constexpr void setRawData(const std::span<u8>& data, u64 offset = 0) {

        arc_assert(data.size() + offset <= this->data.size(), "Cannot copy raw pixel data to smaller image buffer");

        std::copy(data.begin(), data.end(), this->data.data() + offset);

    }

    template<Pixel Q>
    constexpr void setData(const std::span<u8>& data, u64 startPixel = 0) {

        using SrcPixelFormat = PixelFormat<Q>;

        constexpr static u32 SrcBytes = SrcPixelFormat::BytesPerPixel;
        arc_assert(!(data.size() % SrcBytes), "Source data must be divisible by bytes per pixel");

        u32 transferEnd = ((data.size() / SrcBytes) + startPixel) * PixelBytes;
        arc_assert(transferEnd <= this->data.size(), "Cannot convert pixel data to smaller image buffer");

        if constexpr (!Equal<P, Q>) {

            //We gotta convert the pixels during transfer
            for(SizeT i = 0; i < data.size() / SrcBytes; i++) {

                SizeT start = i * SrcBytes;
                auto pixel = SrcPixelFormat::inflate<SrcPixelFormat::InflationType>(data.subspan(start, SrcBytes));
                PixelFormat<P>::deflate(pixel, {this->data.data() + (i + startPixel) * PixelBytes, PixelBytes});

            }

        } else {

            //Copy all
            std::copy(data.begin(), data.end(), this->data + startPixel * PixelBytes);

        }

    }

    template<Pixel Q>
    constexpr Image<Q> convertTo() const {

        Image<Q> image(width, height);
        image.setData(data);

        return image;

    }

    constexpr u32 getWidth() const {
        return width;
    }

    constexpr u32 getHeight() const {
        return height;
    }

    constexpr std::span<u8> getRawData() {
        return data;
    }

    constexpr std::span<const u8> getRawData() const {
        return data;
    }


private:

    u32 width;
    u32 height;
    std::vector<u8> data;

};