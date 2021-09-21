#pragma once

#include "types.h"
#include "util/assert.h"
#include <vector>
#include <span>
#include <array>



enum class Pixel {
    RGB8,
    RGBA8
};


template<Pixel P>
struct PixelFormat {};


template<>
struct PixelFormat<Pixel::RGBA8> {

    constexpr static u32 BytesPerPixel = 4;
    constexpr static u32 Channels = 4;

    using InflationType = u32;

    template<Arithmetic T>
    constexpr static std::array<T, Channels>& inflate(const std::span<const u8>& source) {
        return {source[0], source[1], source[2], source[3]};
    }

    constexpr static void deflate(const std::array<InflationType, Channels>& src, const std::span<u8>& dest) {
        
        for(u32 i = 0; i < Channels; i++) {
            dest[i] = src[i];
        }

    }

};



template<Pixel P>
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
                auto array = SrcPixelFormat::inflate<SrcPixelFormat::InflationType>(data.subspan(start, SrcBytes));
                PixelFormat<P>::deflate(array, {this->data.data() + (i + startPixel) * PixelBytes, PixelBytes});

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