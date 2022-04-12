#include "qoidecoder.hpp"

constexpr u8 hash(u8 r, u8 g, u8 b, u8 a) {
    return (r * 3 + g * 5 + b * 7 + a * 11) % 64;
}

void QOIDecoder::decode(std::span<const u8> data) {
    reader = BinaryReader(data, ByteOrder::Big);

    if (reader.remainingSize() < 14) {
        throw ImageDecoderException("QOI stream size too small");
    }

    u32 magic = reader.read<u32>();

    if (magic != 0x716f6966) {
        throw ImageDecoderException("QOI magic doesn't match");
    }

    u32 width = reader.read<u32>();
    u32 height = reader.read<u32>();

    image = RawImage(width, height);

    u8 channels = reader.read<u8>();
    u8 colorspace = reader.read<u8>();

    PixelRGBA8 prevP;
    prevP.setAlpha(255);

    PixelRGBA8 palette[64];

    Image<Pixel::RGBA8> bufImage(width, height);

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

//                palette[hash(prevP.getRed(), prevP.getBlue(), prevP.getGreen(), prevP.getAlpha())] = prevP;
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
            } else if (tag >> 6 == 0b00) {
                bufImage.setPixel(x, y, palette[tag]);
                prevP = palette[tag];
            } else if (tag >> 6 == 0b01) {
                prevP.setRed((prevP.getRed() + ((tag >> 4) & 0x3) - 2) % 256);
                prevP.setGreen((prevP.getGreen() + ((tag >> 2) & 0x3) - 2) % 256);
                prevP.setBlue((prevP.getBlue() + (tag & 0x3) - 2) % 256);

                bufImage.setPixel(x, y, prevP);
                palette[hash(prevP.getRed(), prevP.getGreen(), prevP.getBlue(), prevP.getAlpha())] = prevP;
            } else if (tag >> 6 == 0b10) {
                if (!reader.remainingSize()) {
                    throw ImageDecoderException("QOI stream size too small");
                }

                u8 dg = (tag & 0x3f) - 32;

                prevP.setGreen(prevP.getGreen() + dg);

                u8 dg_db = reader.read<u8>();

                prevP.setRed(prevP.getRed() + dg + (dg_db >> 4) - 8);
                prevP.setBlue(prevP.getBlue() + dg + (dg_db & 0xf) - 8);

                bufImage.setPixel(x, y, prevP);
                palette[hash(prevP.getRed(), prevP.getGreen(), prevP.getBlue(), prevP.getAlpha())] = prevP;
            } else if (tag >> 6 == 0b11) {
                bufImage.setPixel(x, y, prevP);
                for (u8 rl = 0; rl < (tag & 0x3f); rl++) {
                    x++;
                    if (x == width) {
                        x = 0;
                        y++;
                    }
                    if (y == height) {
                        throw ImageDecoderException("QOI too many pixels");
                    }

                    bufImage.setPixel(x, y, prevP);
                }
            }
        }
    }

    image = bufImage.makeRaw();
    validDecode = true;
}