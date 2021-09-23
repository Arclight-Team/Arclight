#pragma once

#include "types.h"
#include "util/typetraits.h"


//Pixel Types
enum class Pixel {
    RGB8,
    RGBA8
};


struct PixelRGBA8 {

    constexpr PixelRGBA8(u8 r, u8 g, u8 b, u8 a) : r(r), g(g), b(b), a(a) {}

    u8 r, g, b, a;

};



struct PixelRGBA32 {

    constexpr PixelRGBA32(u32 r, u32 g, u32 b, u32 a) : r(r), g(g), b(b), a(a) {}

    constexpr std::array<u32, 4> toArray() const {
        return {r, g, b, a};
    }

    u8 r, g, b, a;

};



//Pixel Formats
template<Pixel P>
struct PixelFormat {};


template<>
struct PixelFormat<Pixel::RGBA8> {

    constexpr static u32 BytesPerPixel = 4;
    constexpr static u32 Channels = 4;
    constexpr static u8 DefaultPixel[BytesPerPixel] = {0x0, 0x0, 0x0, 0xFF};

    using PixelType = PixelRGBA8;

    //Pixel -> Inflated Pixel
    constexpr static PixelRGBA32 inflate(const PixelType& source) {
        return PixelRGBA32(source.r << 24, source.g << 24, source.b << 24, source.a << 24);
    }

    //Inflated Pixel -> Pixel
    template<u32 SrcChannels>
    constexpr static PixelType deflate(const PixelRGBA32& source) {
        return PixelType(source.r >> 24, source.g >> 24, source.b >> 24, source.a >> 24);
    }

    constexpr static PixelType fromBytes(const std::span<const u8>& source) {
        return PixelType(source[0], source[1], source[2], source[3]);
    }

    constexpr static void toBytes(const PixelType& pixel, const std::span<u8>& destination) {

        destination[0] = pixel.r;
        destination[1] = pixel.g;
        destination[2] = pixel.b;
        destination[3] = pixel.a;

    }

};