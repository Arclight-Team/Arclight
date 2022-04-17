/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 qoidecoder.cpp
 */

#include "qoidecoder.hpp"
#include "debug.hpp"



constexpr static u8 hash(u8 r, u8 g, u8 b, u8 a) {
    return (r * 3 + g * 5 + b * 7 + a * 11) % 64;
}



void QOIDecoder::decode(std::span<const u8> data) {

	validDecode = false;

    reader = BinaryReader(data, ByteOrder::Big);

    if (reader.remainingSize() < 14) {
        throw ImageDecoderException("QOI stream size too small");
    }

    u32 magic = reader.read<u32>();

    if (magic != 0x716F6966) {
        throw ImageDecoderException("QOI magic doesn't match");
    }

    u32 width = reader.read<u32>();
    u32 height = reader.read<u32>();

    u8 channels = reader.read<u8>();
    u8 colorspace = reader.read<u8>();

    PixelRGBA8 prevP;
    prevP.setAlpha(255);

    PixelRGBA8 palette[64];

    Image<Pixel::RGBA8> bufImage(width, height);
	u64 pixelCount = bufImage.pixelCount();

    for (u32 y = 0; y < height; y++) {

        for (u32 x = 0; x < width; x++) {

            if (!reader.remainingSize()) {
                throw ImageDecoderException("QOI stream size too small");
            }

            u8 tag = reader.read<u8>();

            if (tag == 0xff) {

                if (reader.remainingSize() < 4) {
                    throw ImageDecoderException("QOI stream size too small");
                }

                prevP.setRed(reader.read<u8>());
                prevP.setGreen(reader.read<u8>());
                prevP.setBlue(reader.read<u8>());
                prevP.setAlpha(reader.read<u8>());

                bufImage.setPixel(x, y, prevP);

                palette[hash(prevP.getRed(), prevP.getGreen(), prevP.getBlue(), prevP.getAlpha())] = prevP;

            } else if (tag == 0xfe) {

                if (reader.remainingSize() < 3) {
                    throw ImageDecoderException("QOI stream size too small");
                }

                prevP.setRed(reader.read<u8>());
                prevP.setGreen(reader.read<u8>());
                prevP.setBlue(reader.read<u8>());

                bufImage.setPixel(x, y, prevP);

                palette[hash(prevP.getRed(), prevP.getGreen(), prevP.getBlue(), prevP.getAlpha())] = prevP;

            } else {

				switch (tag >> 6) {

					case 0b00:

		                bufImage.setPixel(x, y, palette[tag]);
		                prevP = palette[tag];

						break;

					case 0b01:

		                prevP.setRed((prevP.getRed() + ((tag >> 4) & 0x3) - 2) % 256);
		                prevP.setGreen((prevP.getGreen() + ((tag >> 2) & 0x3) - 2) % 256);
		                prevP.setBlue((prevP.getBlue() + (tag & 0x3) - 2) % 256);

		                bufImage.setPixel(x, y, prevP);
		                palette[hash(prevP.getRed(), prevP.getGreen(), prevP.getBlue(), prevP.getAlpha())] = prevP;

						break;

					case 0b10:
						{
							if (!reader.remainingSize()) {
								throw ImageDecoderException("QOI stream size too small");
							}

							u8 dg = (tag & 0x3F) - 32;

							prevP.setGreen(prevP.getGreen() + dg);

							u8 dg_db = reader.read<u8>();

							prevP.setRed(prevP.getRed() + dg + (dg_db >> 4) - 8);
							prevP.setBlue(prevP.getBlue() + dg + (dg_db & 0xF) - 8);

							bufImage.setPixel(x, y, prevP);
							palette[hash(prevP.getRed(), prevP.getGreen(), prevP.getBlue(), prevP.getAlpha())] = prevP;
						}

						break;

					case 0b11:

						u32 runLength = tag & 0x3F;

						if (y * width + x + runLength >= pixelCount) {
							throw ImageDecoderException("QOI too many pixels");
						}

#ifdef ARC_VECTORIZE_X86_SSE2

						__m128i* data = reinterpret_cast<__m128i*>(&bufImage.getImageBuffer()[width * y + x]);

						runLength++;
						u32 vectorCount = runLength / 4;
						u32 scalarCount = runLength % 4;

						__m128i vectorValue = _mm_set1_epi32(prevP.pack());

						for (u32 i = 0; i < vectorCount; i++) {
							_mm_storeu_si128(data++, vectorValue);
						}

						u8* scalarData = Bits::toByteArray(data);

						for (u32 i = 0; i < scalarCount; i++) {

							scalarData[i * 4 + 0] = prevP.getRed();
							scalarData[i * 4 + 1] = prevP.getGreen();
							scalarData[i * 4 + 2] = prevP.getBlue();
							scalarData[i * 4 + 3] = prevP.getAlpha();

						}

						u64 sx = x + runLength - 1;
						y += sx / width;
						x = sx % width;

#else

		                bufImage.setPixel(x, y, prevP);

		                for (u8 rl = 0; rl < runLength; rl++) {

		                    x++;

		                    if (x == width) {

		                        x = 0;
		                        y++;

		                    }

		                    bufImage.setPixel(x, y, prevP);

		                }

#endif

						break;

				}

			}

        }

    }

    image = bufImage.makeRaw();
    validDecode = true;

}



RawImage& QOIDecoder::getImage() {

	if (!validDecode) {
		throw ImageDecoderException("Bad image decode");
	}

	return image;

}