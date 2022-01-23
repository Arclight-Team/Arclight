/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bmp.hpp
 */

#pragma once

#include "image/image.hpp"
#include "stream/inputstream.hpp"
#include "stream/binaryreader.hpp"
#include "util/bool.hpp"
#include "util/typetraits.hpp"
#include "arcconfig.hpp"



namespace BMP {

    constexpr inline u32 headerSize = 14;
    constexpr inline u32 infoHeaderSize = 40;
    constexpr inline u32 infoHeaderSizeV4 = 108;
    constexpr inline u32 infoHeaderSizeV5 = 124;
    constexpr inline u16 headerMagic = 0x4D42;  //BM

    constexpr inline u32 csCalibratedRGB = 0;
    constexpr inline u32 csSRGB = 0x73524742;
    constexpr inline u32 csSystemDefault = 0x57696E20;
    constexpr inline u32 csLinkedProfile = 0x4C494E4B;
    constexpr inline u32 csEmbeddedProfile = 0x4D424544;

    constexpr inline u32 riBusiness = 1;
    constexpr inline u32 riGraphics = 2;
    constexpr inline u32 riImages = 4;
    constexpr inline u32 riColorimetric = 8;

    //Bitmap header
    struct BitmapHeader {

        u16 signature;
        u32 fileSize;
        u16 reserved1;
        u16 reserved2;
        u32 dataOffset;

    };

    //Color space point in CIE coordinates (Q2.30 fixed-point)
    struct ColorSpacePoint {

        u32 x;
        u32 y;
        u32 z;

    };

    //Bitmap info header, up to V5
    struct BitmapInfoHeader {

        //Standard fields
        u32 size;
        i32 width;
        i32 height;
        u16 planes;
        u16 bitsPerPixel;
        u32 compression;
        u32 imageSize;
        i32 pixelsPerMeterX;
        i32 pixelsPerMeterY;
        u32 paletteColors;
        u32 importantColors;

        //V4 fields
        u32 redMask;
        u32 greenMask;
        u32 blueMask;
        u32 alphaMask;
        u32 colorSpace;
        ColorSpacePoint endpoint0;
        ColorSpacePoint endpoint1;
        ColorSpacePoint endpoint2;
        u32 redGamma;       //Gamma in Q16.15
        u32 greenGamma;
        u32 blueGamma;

        //V5 fields
        u32 renderIntent;
        u32 profileDataOffset;  //From this header
        u32 profileDataSize;
        u32 reserved;

    };

    enum class Version {
        Info,
        V4,
        V5
    };

    enum class Compression {
        None,
        RLE8,
        RLE4,
        Masked,
        JPEG,
        PNG
    };

    template<Pixel P>
    Image<P> loadBitmap(InputStream& stream) {

        using Format = PixelFormat<P>;
        using PixelType = typename PixelType<P>::Type;

        BinaryReader reader(stream, true, ByteOrder::Little);
        SizeT streamSize = stream.getSize();

        if(streamSize < headerSize + infoHeaderSize) {
            Log::error("Bitmap Loader", "Failed to load bitmap: Bitmap stream size too small");
            return Image<P>();
        }

        //File loading
        BitmapHeader header;
        header.signature = reader.read<u16>();
        header.fileSize = reader.read<u32>();
        header.reserved1 = reader.read<u16>();
        header.reserved2 = reader.read<u16>();
        header.dataOffset = reader.read<u32>();

#ifdef ARC_IMAGE_DEBUG
        Log::info("Bitmap Loader", "Header: 0x%X, Size: 0x%X, Reserved1: 0x%X, Reserved2: 0x%X, Offset: 0x%X", header.signature, header.fileSize, header.reserved1, header.reserved2, header.dataOffset);
#endif

        if(header.signature != headerMagic) {
            Log::error("Bitmap Loader", "Failed to load bitmap: Stream does not represent a bitmap file");
            return Image<P>();
        }

        BitmapInfoHeader infoHeader;
        infoHeader.size = reader.read<u32>();

        Version version = Version::Info;

        switch (infoHeader.size) {

            case infoHeaderSize:
                version = Version::Info;
                break;

            case infoHeaderSizeV4:
                version = Version::V4;
                break;

            case infoHeaderSizeV5:
                version = Version::V5;
                break;

            default:
                Log::error("Bitmap Loader", "Failed to load bitmap: Invalid/Unsupported bitmap version");
                return Image<P>();

        }

        infoHeader.width = reader.read<i32>();
        infoHeader.height = reader.read<i32>();
        infoHeader.planes = reader.read<u16>();
        infoHeader.bitsPerPixel = reader.read<u16>();
        infoHeader.compression = reader.read<u32>();
        infoHeader.imageSize = reader.read<u32>();
        infoHeader.pixelsPerMeterX = reader.read<i32>();
        infoHeader.pixelsPerMeterY = reader.read<i32>();
        infoHeader.paletteColors = reader.read<u32>();
        infoHeader.importantColors = reader.read<u32>();

#ifdef ARC_IMAGE_DEBUG
        Log::info("Bitmap Loader", "InfoSize: 0x%X, Width: 0x%X, Height: 0x%X, Planes: 0x%X, BPP: 0x%X, Compression: 0x%X, ImageSize: 0x%X, PPMX: 0x%X, PPMY: 0x%X, PaletteColors: 0x%X, ImportantColors: 0x%X", 
        infoHeader.size, infoHeader.width, infoHeader.height, infoHeader.planes, infoHeader.bitsPerPixel, infoHeader.compression, infoHeader.imageSize, infoHeader.pixelsPerMeterX, infoHeader.pixelsPerMeterY, infoHeader.paletteColors, infoHeader.importantColors);
#endif

        if(version == Version::V4 || version == Version::V5) {

            if(streamSize < headerSize + infoHeaderSizeV4) {
                Log::error("Bitmap Loader", "Failed to load bitmap: Stream size too small (V4)");
                return Image<P>();
            }

            infoHeader.redMask = reader.read<u32>();
            infoHeader.greenMask = reader.read<u32>();
            infoHeader.blueMask = reader.read<u32>();
            infoHeader.alphaMask = reader.read<u32>();
            infoHeader.colorSpace = reader.read<u32>();

            infoHeader.endpoint0.x = reader.read<u32>();
            infoHeader.endpoint0.y = reader.read<u32>();
            infoHeader.endpoint0.z = reader.read<u32>();
            infoHeader.endpoint1.x = reader.read<u32>();
            infoHeader.endpoint1.y = reader.read<u32>();
            infoHeader.endpoint1.z = reader.read<u32>();
            infoHeader.endpoint2.x = reader.read<u32>();
            infoHeader.endpoint2.y = reader.read<u32>();
            infoHeader.endpoint2.z = reader.read<u32>();

            infoHeader.redGamma = reader.read<u32>();
            infoHeader.greenGamma = reader.read<u32>();
            infoHeader.blueGamma = reader.read<u32>();

#ifdef ARC_IMAGE_DEBUG
            Log::info("Bitmap Loader", "RedMask: 0x%X, GreenMask: 0x%X, BlueMask: 0x%X, AlphaMask: 0x%X, RedGamma: 0x%X, GreenGamma: 0x%X, BlueGamma: 0x%X", 
            infoHeader.redMask, infoHeader.greenMask, infoHeader.blueMask, infoHeader.alphaMask, infoHeader.redGamma, infoHeader.greenGamma, infoHeader.blueGamma);

            Log::info("Bitmap Loader", "ColorSpace: 0x%X, CIEEndpoint0: [0x%X, 0x%X, 0x%X], CIEEndpoint1: [0x%X, 0x%X, 0x%X], CIEEndpoint2: [0x%X, 0x%X, 0x%X]", 
            infoHeader.colorSpace, infoHeader.endpoint0.x, infoHeader.endpoint0.y, infoHeader.endpoint0.z, infoHeader.endpoint1.x, infoHeader.endpoint1.y, infoHeader.endpoint1.z, infoHeader.endpoint2.x, infoHeader.endpoint2.y, infoHeader.endpoint2.z);
#endif

            if(version == Version::V5) {

                if(streamSize < headerSize + infoHeaderSizeV5) {
                    Log::error("Bitmap Loader", "Failed to load bitmap: Stream size too small (V5)");
                    return Image<P>();
                }

                infoHeader.renderIntent = reader.read<u32>();
                infoHeader.profileDataOffset = reader.read<u32>();
                infoHeader.profileDataSize = reader.read<u32>();
                infoHeader.reserved = reader.read<u32>();

#ifdef ARC_IMAGE_DEBUG
                Log::info("Bitmap Loader", "RenderIntent: 0x%X, ProfileDataOffset: 0x%X, ProfileDataSize: 0x%X, Reserved: 0x%X", 
                infoHeader.renderIntent, infoHeader.profileDataOffset, infoHeader.profileDataSize, infoHeader.reserved);
#endif

            }

        }


        //Integrity checking
        if(infoHeader.width < 0 || infoHeader.planes != 1 || Bool::none(infoHeader.bitsPerPixel, 0, 1, 4, 8, 16, 24, 32) || infoHeader.compression > 5) {
            Log::error("Bitmap Loader", "Failed to load bitmap: Invalid parameters");
            return Image<P>();
        }

        Compression compression = static_cast<Compression>(infoHeader.compression);

        if(compression == Compression::JPEG || compression == Compression::PNG) {
            Log::error("Bitmap Loader", "Failed to load bitmap: JPEG/PNG compression unsupported");
            return Image<P>();
        }

        if((version == Version::V4 && infoHeader.colorSpace != csCalibratedRGB) ||
           (version == Version::V5 && Bool::none(infoHeader.colorSpace, csCalibratedRGB, csSRGB, csSystemDefault, csLinkedProfile, csEmbeddedProfile))) {
            Log::error("Bitmap Loader", "Failed to load bitmap: Illegal color space parameter");
            return Image<P>();
        }

        if(version == Version::V5 && Bool::none(infoHeader.renderIntent, 0, riBusiness, riGraphics, riImages, riColorimetric)) {
            Log::error("Bitmap Loader", "Failed to load bitmap: Illegal render intent");
            return Image<P>();
        }

        bool topDown = infoHeader.height < 0;
        infoHeader.height = Math::abs(infoHeader.height);

        Image<P> image(infoHeader.width, infoHeader.height);

        //Color reading
        switch(compression) {

            case Compression::None:
            {

                if(infoHeader.bitsPerPixel >= 16) {

                    //Direct
                    u32 bytesPerPixel = infoHeader.bitsPerPixel / 8;
                    u32 rowBytes = image.getWidth() * bytesPerPixel;
                    u32 rowBytesAligned = Math::alignUp(rowBytes, 4);
                    u32 rowAlign = rowBytesAligned - rowBytes;

                    if(streamSize < header.dataOffset + rowBytesAligned * image.getHeight()) {
                        Log::error("Bitmap Loader", "Failed to load bitmap: Stream size too small");
                        return Image<P>();
                    }

                    stream.seekTo(header.dataOffset);
                    
                    u8 data[4];

                    auto loadData = [&]<u32 N, class T>() {
                        
                        for(u32 y = 0; y < image.getHeight(); y++) {

                            u32 ry = topDown ? image.getHeight() - y - 1 : y;

                            for(u32 x = 0; x < image.getWidth(); x++) {

                                reader.read<u8>({data, N});
                                image.setPixel(x, ry, PixelConverter::convert<P>(T({data, N})));

                            }

                            if constexpr (N == 2 || N == 3) {

                                if(rowAlign) {
                                    stream.seek(rowAlign);
                                }

                            }

                        }

                    };

                    switch(infoHeader.bitsPerPixel) {

                        case 16:
                            loadData.template operator()<2, PixelBGR5>();
                            break;

                        case 24:
                            loadData.template operator()<3, PixelBGR8>();
                            break;

                        default:
                        case 32:
                            loadData.template operator()<4, PixelBGRA8>();
                            break;

                    }

                } else {

                    if(infoHeader.bitsPerPixel == 0) {
                        Log::error("Bitmap Loader", "Failed to load bitmap: Bits per pixel cannot be 0 for non JPEG/PNG images");
                        return Image<P>();
                    }

                    //Palette
                    std::vector<PixelType> palette;
                    u32 colorCount = infoHeader.paletteColors ? infoHeader.paletteColors : 1 << infoHeader.bitsPerPixel;
                    
                    SizeT streamPos = stream.getPosition();

                    if(streamSize < streamPos + colorCount * 4 * 4) {
                        Log::error("Bitmap Loader", "Failed to load bitmap: Stream size too small");
                        return Image<P>();
                    }

                    for(u32 i = 0; i < colorCount; i++) {
                        palette.push_back(PixelConverter::convert<P>(PixelBGRA8(reader.read<u32>())));
                    }

                    u32 bitsPerRow = infoHeader.width * infoHeader.bitsPerPixel;
                    u32 bitsPerRowAligned = Math::alignUp(bitsPerRow, 32);
                    u32 totalImageDataSize = (bitsPerRowAligned * infoHeader.height) / 8;

                    u32 leftoverPixels = (bitsPerRowAligned - bitsPerRow) / infoHeader.bitsPerPixel;
                    u32 leftoverX = image.getWidth() - leftoverPixels;

                    u32 rowReads = bitsPerRow / 32;

                    if(streamSize < header.dataOffset + totalImageDataSize) {
                        Log::error("Bitmap Loader", "Failed to load bitmap: Stream size too small");
                        return Image<P>();
                    }

                    stream.seekTo(header.dataOffset);

                    auto loadData = [&]<u32 BPP>() {

                        constexpr static u32 pixelsPerRead = 32 / BPP;
                        constexpr static u32 pixelsPerByte = pixelsPerRead / 4;
                        constexpr static u32 bytemask = ((1 << BPP) - 1) << (8 - BPP);

                        for(u32 y = 0; y < image.getHeight(); y++) {

                            u32 ry = topDown ? image.getHeight() - y - 1 : y;

                            for(u32 x = 0; x < rowReads; x++) {

                                u32 pixelData = reader.read<u32>();

                                for(u32 i = 0; i < 4; i++) {

                                    u32 mask = bytemask;

                                    for(u32 j = 0; j < pixelsPerByte; j++) {
                                        
                                        image.setPixel(x * pixelsPerRead + i * pixelsPerByte + j, ry, palette[(pixelData >> (8 * i) & mask) >> (8 - (j + 1) * BPP)]);
                                        mask >>= BPP;

                                    }

                                }

                            }

                            u32 k = 0;
                            u32 pixelData = reader.read<u32>();

                            for(u32 i = 0; i < Math::alignUp(leftoverPixels, 8) / 8; i++) {

                                u32 mask = bytemask;

                                for(u32 j = 0; j < pixelsPerByte; j++) {
                                    
                                    image.setPixel(leftoverX + k, ry, palette[(pixelData >> (8 * i) & mask) >> (8 - (j + 1) * BPP)]);
                                    mask >>= BPP;

                                    if(++k == leftoverPixels) {
                                        break;
                                    }

                                }

                            }

                        }

                    };

                    switch(infoHeader.bitsPerPixel) {

                        case 1:
                            loadData.template operator()<1>();
                            break;

                        case 4:
                            loadData.template operator()<4>();
                            break;

                        default:
                        case 8:
                            loadData.template operator()<8>();
                            break;

                    }

                }

            }
            break;

            case Compression::RLE4:
            case Compression::RLE8:
            {

                if(topDown) {
                    Log::error("Bitmap Loader", "Failed to load bitmap: Run-Length encoded bitmap cannot be top-down");
                    return Image<P>();
                }

                //Palette
                std::vector<PixelType> palette;
                u32 colorCount = infoHeader.paletteColors ? infoHeader.paletteColors : 1 << infoHeader.bitsPerPixel;
                
                SizeT streamPos = stream.getPosition();

                if(streamSize < streamPos + colorCount * 4 * 4) {
                    Log::error("Bitmap Loader", "Failed to load bitmap: Stream size too small");
                    return Image<P>();
                }

                for(u32 i = 0; i < colorCount; i++) {
                    palette.push_back(PixelConverter::convert<P>(PixelBGRA8(reader.read<u32>())));
                }

                stream.seekTo(header.dataOffset);

                u8 ctrl[2];
                u32 x = 0;
                u32 y = 0;
                bool eob = false;
                constexpr u32 skipColorIdx = 0;

                auto loadData = [&]<u32 N>() {

                    constexpr static bool IsRLE4 = N == 4;
                    constexpr static u32 directIndexBytes = IsRLE4 ? 0x80 : 0xFF;

                    while(!eob) {

                        reader.read(std::span<u8>{ctrl, 2});

                        if(ctrl[0]) {

                            //n times index
                            for(u32 i = 0; i < ctrl[0]; i++) {

                                if constexpr (IsRLE4) {

                                    u32 shift = (!(i & 1) * 4);
                                    image.setPixel(x++, y, palette[(ctrl[1] & (0xF << shift)) >> shift]);

                                } else {

                                    image.setPixel(x++, y, palette[ctrl[1]]);

                                }


                            }

                        } else {

                            switch(ctrl[1]) {

                                case 0:

                                    //End of line
                                    x = 0;
                                    y++;
                                    break;

                                case 1:

                                    //End of bitmap
                                    eob = true;
                                    break;

                                case 2:

                                    //Delta
                                    reader.read(std::span<u8>{ctrl, 2});

                                    for(u32 i = 0; i < ctrl[0]; i++) {
                                        image.setPixel(x++, y, palette[skipColorIdx]);
                                    }

                                    for(u32 i = 0; i < ctrl[1]; i++) {

                                        for(x = 0; x < image.getWidth(); x++) {
                                            image.setPixel(x, y, palette[skipColorIdx]);
                                        }

                                        y++;

                                    }

                                    break;

                                default:
                                {
                                    //Direct
                                    u32 pixelCount = ctrl[1];
                                    u32 byteCount = IsRLE4 ? (pixelCount + 1) / 2 : pixelCount;
                                    u8 indices[directIndexBytes];

                                    reader.read(std::span<u8>{indices, byteCount});

                                    if(byteCount & 1) {
                                        stream.seekTo(1, StreamBase::SeekMode::Current);
                                    }

                                    for(u32 i = 0; i < pixelCount; i++) {

                                        if constexpr (IsRLE4) {

                                            u32 shift = (!(i & 1) * 4);
                                            image.setPixel(x++, y, palette[(indices[i / 2] & (0xF << shift)) >> shift]);

                                        } else {

                                            image.setPixel(x++, y, palette[indices[i]]);

                                        }

                                    }

                                }
                                break;

                            }

                        }

                    }

                };

                if (compression == Compression::RLE4) {
                    loadData.template operator()<4>();
                } else {
                    loadData.template operator()<8>();
                }

            }
            break;

            case Compression::Masked:
            {

                if(Bool::none(infoHeader.bitsPerPixel, 16, 32)) {
                    Log::error("Bitmap Loader", "Failed to load bitmap: Masked bitmaps must be 16bpp or 32bpp");
                    return Image<P>();
                }

                u32 redMask, greenMask, blueMask, alphaMask;

                if(version == Version::Info) {

                    if(streamSize < headerSize + infoHeaderSize + 3 * 4) {
                        Log::error("Bitmap Loader", "Failed to load bitmap: Stream size too small");
                        return Image<P>();
                    }

                    redMask = reader.read<u32>();
                    greenMask = reader.read<u32>();
                    blueMask = reader.read<u32>();
                    alphaMask = 0;

                } else {

                    redMask = infoHeader.redMask;
                    greenMask = infoHeader.greenMask;
                    blueMask = infoHeader.blueMask;
                    alphaMask = infoHeader.alphaMask;

                }

                u32 redShift = Bits::ctz(redMask);
                u32 greenShift = Bits::ctz(greenMask);
                u32 blueShift = Bits::ctz(blueMask);
                u32 alphaShift = Bits::ctz(alphaMask);

                u32 rowBytes = image.getWidth() * infoHeader.bitsPerPixel / 8;
                u32 rowBytesAligned = Math::alignUp(rowBytes, 4);
                bool rowAlign = rowBytesAligned - rowBytes;

                if(streamSize < header.dataOffset + rowBytesAligned * image.getHeight()) {
                    Log::error("Bitmap Loader", "Failed to load bitmap: Stream size too small");
                    return Image<P>();
                }

                stream.seekTo(header.dataOffset);

                auto loadData = [&]<u32 N>() {

                    using T = TT::Conditional<N != 16, u32, u16>;
                        
                    for(u32 y = 0; y < image.getHeight(); y++) {

                        u32 ry = topDown ? image.getHeight() - y - 1 : y;

                        for(u32 x = 0; x < image.getWidth(); x++) {

                            T pixelData = reader.read<T>();
                            auto pixel = PixelConverter::convert<P, T>(pixelData, redMask, redShift, greenMask, greenShift, blueMask, blueShift, alphaMask, alphaShift);
                            image.setPixel(x, ry, pixel);

                        }

                        if constexpr (N == 16) {

                            if(rowAlign) {
                                stream.seekTo(2, StreamBase::SeekMode::Current);
                            }

                        } 

                    }

                };

                if(infoHeader.bitsPerPixel == 16) {
                    loadData.template operator()<16>();
                } else {
                    loadData.template operator()<32>();
                }

            }
            break;

            case Compression::JPEG:
            case Compression::PNG:
            default:
                break;

        }

        return image;

    }

}