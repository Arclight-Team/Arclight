#pragma once

#include "types.h"
#include "util/typetraits.h"


//Pixel Types
enum class Pixel {
    RGB8,
    RGBA8
};



//Pixel Formats
template<Pixel P>
struct PixelFormat {};


template<>
struct PixelFormat<Pixel::RGBA8> {

    constexpr static u32 BytesPerPixel = 4;
    constexpr static u32 Channels = 4;

    using InflationType = u32;
    //using PixelContainer = PixelRGBA;

    template<Arithmetic T>
    constexpr static std::array<u32, Channels> inflate(const std::span<const u8>& source) {
        return {source[0] << 24, source[1] << 24, source[2] << 24, source[3] << 24};
    }

    constexpr static void deflate(const std::array<u32, Channels>& src, const std::span<u8>& dest) {
        
        dest[0] = src[0] >> 24;
        dest[1] = src[1] >> 24;
        dest[2] = src[2] >> 24;
        dest[3] = src[3] >> 24;

    }

};