#pragma once

#include "types.h"

#include <vector>
#include <span>
#include <array>


/*
enum class PixelType {
    RGB8,
    RGBA8
};


template<PixelType Type, u32 Channels, u32 BytesPerPixel, bool Alpha>
struct PixelFormat {

public:

    constexpr static u32 getChannelCount() const {
        return channels;
    }

    constexpr static bool hasAlphaChannel() const {
        return hasAlpha;
    }

    constexpr static bool isMonochrome() const {
        return getChannelCount() == 1;
    }

    constexpr static u32 bytesPerPixel() const {
        return bpp;
    }

    constexpr static u8 nullByte() const {
        return 0;
    }

private:

    constexpr static u32 channels = Channels;
    constexpr static u32 bpp = BytesPerPixel;
    constexpr static bool hasAlpha = Alpha;

};

template<>
struct PixelFormat<PixelType::RGB8, 3, 2, false> {

    constexpr static void decode(u8* stream, u32& c0, u32& c1, u32& c2, u32& c3) {

        constexpr u32 shift = 24;
        c0 = stream[0] << shift;
        c1 = stream[1] << shift;
        c2 = stream[2] << shift;

    }

    constexpr static void encode(u8* stream, u32& c0, u32& c1, u32& c2, u32& c3) {

        constexpr u32 shift = 24;
        c0 = stream[0] >> shift;
        c1 = stream[1] >> shift;
        c2 = stream[2] >> shift;

    }

};


#define ARC_IMAGE_CONVERT_IMPL(type) \
case type: \
 \
    for(SizeT i = 0; i < w * h; i++) { \
        u32 a, b, c, d; \
        PixelFormat<type>::decode(data.data() + i * 3, a, b, c, d); \
        PixelFormat<DestType>::encode(data.data() + i * 3, a, b, c, d); \
    } 


class Image {

public:

    Image();
    Image(u32 width, u32 height, PixelType type, const std::span<u8>& source);

    void setData(const std::span<u8>& source);
    void setData(u32 width, u32 height, PixelType type, const std::span<u8>& source);

    std::span<const u8> getData() const;
    u64 getDataSize() const;

    template<PixelType DestType>
    void convert() {

        switch(type) {


        }

    }

private:

    u32 width;
    u32 height;
    PixelType type;
    std::vector<u8> data;

};
*/