/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bitmapdecoder.cpp
 */

#include "bitmapdecoder.hpp"
#include "util/bool.hpp"



void BitmapDecoder::decode(std::span<const u8> data) {

	validDecode = false;

	reader = BinaryReader(data, ByteOrder::Little);

	SizeT dataOffset = parseHeader();
	parseInfoHeader();


	//Color reading
	switch(bitmap.compression) {

		case Bitmap::Compression::None:
		{

			if (bitmap.bitsPerPixel >= 16) {

				//Direct
				reader.seekTo(dataOffset);
				decodeDirect();

			} else {

				//Palette
				if(bitmap.bitsPerPixel == 0) {
					throw ImageDecoderException("Bits per pixel cannot be 0 for non JPEG/PNG images");
				}

				loadPalette();

				reader.seekTo(dataOffset);

				decodeIndexed();

			}

		}
		break;

		case Bitmap::Compression::RLE4:
		case Bitmap::Compression::RLE8:
		{

			//Run-Length encoded
			if (bitmap.topDown) {
				throw ImageDecoderException("Run-Length encoded bitmap cannot be top-down");
			}

			loadPalette();

			reader.seekTo(dataOffset);

			decodeRLE();

		}
		break;

		case Bitmap::Compression::Masked:
		{

			if (Bool::none(bitmap.bitsPerPixel, 16, 32)) {
				throw ImageDecoderException("Masked bitmaps must be 16bpp or 32bpp");
			}

			if (bitmap.version == Bitmap::Version::Info) {

				if (reader.remainingSize() < 12) {
					throw ImageDecoderException("Stream size too small");
				}

				bitmap.redMask = reader.read<u32>();
				bitmap.greenMask = reader.read<u32>();
				bitmap.blueMask = reader.read<u32>();
				bitmap.alphaMask = 0;

			}

			reader.seekTo(dataOffset);

			decodeMasked();

		}
		break;

		case Bitmap::Compression::JPEG:
		case Bitmap::Compression::PNG:
		default:
			break;

	}

	validDecode = true;

}



SizeT BitmapDecoder::parseHeader() {

	if (reader.remainingSize() < 14) {
		throw ImageDecoderException("Bitmap stream size too small");
	}

	u16 signature = reader.read<u16>();
	u16 fileSize = reader.read<u32>();
	u16 reserved1 = reader.read<u16>();
	u16 reserved2 = reader.read<u16>();
	u32 dataOffset = reader.read<u32>();

#ifdef ARC_IMAGE_DEBUG
	Log::info("Bitmap Loader", "Header: 0x%X, Size: 0x%X, Reserved1: 0x%X, Reserved2: 0x%X, Offset: 0x%X", signature, fileSize, reserved1, reserved2, dataOffset);
#endif

	constexpr u16 headerMagic = 0x4D42;

	if (signature != headerMagic) {
		throw ImageDecoderException("Stream does not represent a bitmap file");
	}

	if (reader.remainingSize() < dataOffset) {
		throw ImageDecoderException("Bitmap stream size too small");
	}

	return dataOffset;

}



void BitmapDecoder::parseInfoHeader() {

	if (reader.remainingSize() < 40) {
		throw ImageDecoderException("Bitmap stream size too small");
	}

	u32 headerSize = reader.read<u32>();

	bitmap.version = Bitmap::Version::Info;

	switch (headerSize) {

		case 40:
			bitmap.version = Bitmap::Version::Info;
			break;

		case 108:
			bitmap.version = Bitmap::Version::V4;
			break;

		case 124:
			bitmap.version = Bitmap::Version::V5;
			break;

		default:
			throw ImageDecoderException("Invalid/Unsupported bitmap version");

	}

	i32 width = reader.read<i32>();
	i32 height = reader.read<i32>();
	u16 planes = reader.read<u16>();

	bitmap.bitsPerPixel = reader.read<u16>();

	u32 compression = reader.read<u32>();
	u32 imageSize = reader.read<u32>();

	bitmap.pixelsPerMeterX = reader.read<i32>();
	bitmap.pixelsPerMeterY = reader.read<i32>();
	bitmap.paletteColors = reader.read<u32>();
	bitmap.importantColors = reader.read<u32>();

#ifdef ARC_IMAGE_DEBUG
	Log::info("Bitmap Loader", "InfoSize: 0x%X, Width: 0x%X, Height: 0x%X, Planes: 0x%X, BPP: 0x%X, Compression: 0x%X, ImageSize: 0x%X, PPMX: 0x%X, PPMY: 0x%X, PaletteColors: 0x%X, ImportantColors: 0x%X",
	headerSize, width, height, planes, bitmap.bitsPerPixel, compression, imageSize, bitmap.pixelsPerMeterX, bitmap.pixelsPerMeterY, bitmap.paletteColors, bitmap.importantColors);
#endif

	//Integrity checking
	if(width < 0 || planes != 1 || Bool::none(bitmap.bitsPerPixel, 0, 1, 4, 8, 16, 24, 32) || compression > 5) {
		throw ImageDecoderException("Invalid parameters");
	}

	bitmap.image = RawImage(width, Math::abs(height));
	bitmap.topDown = height < 0;
	bitmap.compression = static_cast<Bitmap::Compression>(compression);

	if (bitmap.version == Bitmap::Version::V4 || bitmap.version == Bitmap::Version::V5) {

		if(reader.remainingSize() < 68) {
			throw ImageDecoderException("Stream size too small (V4)");
		}

		bitmap.redMask = reader.read<u32>();
		bitmap.greenMask = reader.read<u32>();
		bitmap.blueMask = reader.read<u32>();
		bitmap.alphaMask = reader.read<u32>();

		u32 colorSpace = reader.read<u32>();

		bitmap.endpoint0.x = reader.read<u32>();
		bitmap.endpoint0.y = reader.read<u32>();
		bitmap.endpoint0.z = reader.read<u32>();
		bitmap.endpoint1.x = reader.read<u32>();
		bitmap.endpoint1.y = reader.read<u32>();
		bitmap.endpoint1.z = reader.read<u32>();
		bitmap.endpoint2.x = reader.read<u32>();
		bitmap.endpoint2.y = reader.read<u32>();
		bitmap.endpoint2.z = reader.read<u32>();

		bitmap.redGamma = reader.read<u32>();
		bitmap.greenGamma = reader.read<u32>();
		bitmap.blueGamma = reader.read<u32>();

#ifdef ARC_IMAGE_DEBUG
		Log::info("Bitmap Loader", "RedMask: 0x%X, GreenMask: 0x%X, BlueMask: 0x%X, AlphaMask: 0x%X, RedGamma: 0x%X, GreenGamma: 0x%X, BlueGamma: 0x%X",
		bitmap.redMask, bitmap.greenMask, bitmap.blueMask, bitmap.alphaMask, bitmap.redGamma, bitmap.greenGamma, bitmap.blueGamma);

		Log::info("Bitmap Loader", "ColorSpace: 0x%X, CIEEndpoint0: [0x%X, 0x%X, 0x%X], CIEEndpoint1: [0x%X, 0x%X, 0x%X], CIEEndpoint2: [0x%X, 0x%X, 0x%X]",
		colorSpace, bitmap.endpoint0.x, bitmap.endpoint0.y, bitmap.endpoint0.z, bitmap.endpoint1.x, bitmap.endpoint1.y, bitmap.endpoint1.z, bitmap.endpoint2.x, bitmap.endpoint2.y, bitmap.endpoint2.z);
#endif

		switch (colorSpace) {

			case static_cast<u32>(Bitmap::ColorSpace::SRGB):
			case static_cast<u32>(Bitmap::ColorSpace::SystemDefault):
			case static_cast<u32>(Bitmap::ColorSpace::LinkedProfile):
			case static_cast<u32>(Bitmap::ColorSpace::EmbeddedProfile):

				if (bitmap.version != Bitmap::Version::V5) {
					throw ImageDecoderException("Illegal color space parameter");
				}

			case static_cast<u32>(Bitmap::ColorSpace::Calibrated):
				break;

			default:
				throw ImageDecoderException("Illegal color space parameter");

		}

		bitmap.colorSpace = static_cast<Bitmap::ColorSpace>(colorSpace);

		if (bitmap.version == Bitmap::Version::V5) {

			if (reader.remainingSize() < 16) {
				throw ImageDecoderException("Stream size too small (V5)");
			}

			u32 renderIntent = reader.read<u32>();

			u32 profileDataOffset = reader.read<u32>();
			u32 profileDataSize = reader.read<u32>();
			u32 reserved = reader.read<u32>();

#ifdef ARC_IMAGE_DEBUG
			Log::info("Bitmap Loader", "RenderIntent: 0x%X, ProfileDataOffset: 0x%X, ProfileDataSize: 0x%X, Reserved: 0x%X",
			renderIntent, profileDataOffset, profileDataSize, reserved);
#endif

			if (Bool::none(renderIntent,    static_cast<u32>(Bitmap::RenderIntent::None), static_cast<u32>(Bitmap::RenderIntent::AbsoluteColorimetric), static_cast<u32>(Bitmap::RenderIntent::Perceptual),
			                                    static_cast<u32>(Bitmap::RenderIntent::RelativeColorimetric), static_cast<u32>(Bitmap::RenderIntent::Saturation))) {
				throw ImageDecoderException("Illegal render intent");
			}

		}

	}

	if(bitmap.compression == Bitmap::Compression::JPEG || bitmap.compression == Bitmap::Compression::PNG) {
		throw ImageDecoderException("JPEG/PNG compression unsupported");
	}

}



void BitmapDecoder::loadPalette() {

	u32 colorCount = bitmap.paletteColors ? bitmap.paletteColors : 1 << bitmap.bitsPerPixel;

	if (reader.remainingSize() < colorCount * 4 * 4) {
		throw ImageDecoderException("Stream size too small");
	}

	palette.clear();
	palette.reserve(colorCount);

	for(u32 i = 0; i < colorCount; i++) {
		palette.emplace_back(reader.read<u32>());
	}

}



void BitmapDecoder::decodeDirect() {

	u32 bytesPerPixel = bitmap.bitsPerPixel / 8;
	u32 rowBytes = bitmap.image.getWidth() * bytesPerPixel;
	u32 rowBytesAligned = Math::alignUp(rowBytes, 4);
	u32 rowAlign = rowBytesAligned - rowBytes;

	if(reader.remainingSize() < rowBytesAligned * bitmap.image.getHeight()) {
		throw ImageDecoderException("Stream size too small");
	}

	u8 data[4];

	auto loadData = [&]<u32 N>() {

		constexpr Pixel P = N == 2 ? Pixel::BGR5 : (N == 3 ? Pixel::BGR8 : Pixel::BGRA8);
		using PixelT = PixelType<P>;

		Image<P> image(bitmap.image.getWidth(), bitmap.image.getHeight());

		for(u32 y = 0; y < image.getHeight(); y++) {

			u32 ry = bitmap.topDown ? image.getHeight() - y - 1 : y;

			for(u32 x = 0; x < image.getWidth(); x++) {

				reader.read<u8>({data, N});
				image.setPixel(x, ry, PixelT({data, N}));

			}

			if constexpr (N == 2 || N == 3) {

				if(rowAlign) {
					reader.seek(rowAlign);
				}

			}

		}

		bitmap.image = image.makeRaw();

	};

	switch(bitmap.bitsPerPixel) {

		case 16:
			loadData.template operator()<2>();
			break;

		case 24:
			loadData.template operator()<3>();
			break;

		default:
		case 32:
			loadData.template operator()<4>();
			break;

	}

}



void BitmapDecoder::decodeIndexed() {

	u32 width = bitmap.image.getWidth();
	u32 height = bitmap.image.getHeight();

	u32 bitsPerRow = width * bitmap.bitsPerPixel;
	u32 bitsPerRowAligned = Math::alignUp(bitsPerRow, 32);
	u32 totalImageDataSize = (bitsPerRowAligned * height) / 8;

	u32 leftoverPixels = (bitsPerRowAligned - bitsPerRow) / bitmap.bitsPerPixel;
	u32 leftoverX = width - leftoverPixels;

	u32 rowReads = bitsPerRow / 32;

	if (reader.remainingSize() < totalImageDataSize) {
		throw ImageDecoderException("Stream size too small");
	}

	auto loadData = [&]<u32 BPP>() {

		constexpr u32 pixelsPerRead = 32 / BPP;
		constexpr u32 pixelsPerByte = pixelsPerRead / 4;
		constexpr u32 bytemask = ((1 << BPP) - 1) << (8 - BPP);

		Image<Pixel::BGRA8> image(width, height);


		for (u32 y = 0; y < height; y++) {

			u32 ry = bitmap.topDown ? height - y - 1 : y;

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

			for (u32 i = 0; i < Math::alignUp(leftoverPixels, 8) / 8; i++) {

				u32 mask = bytemask;

				for (u32 j = 0; j < pixelsPerByte; j++) {

					image.setPixel(leftoverX + k, ry, palette[(pixelData >> (8 * i) & mask) >> (8 - (j + 1) * BPP)]);
					mask >>= BPP;

					if (++k == leftoverPixels) {
						break;
					}

				}

			}

		}

		bitmap.image = image.makeRaw();

	};

	switch (bitmap.bitsPerPixel) {

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



void BitmapDecoder::decodeRLE() {

	u8 ctrl[2];
	u32 x = 0;
	u32 y = 0;
	bool eob = false;
	constexpr u32 skipColorIdx = 0;

	auto loadData = [&]<u32 N>() {

		Image<Pixel::BGRA8> image(bitmap.image.getWidth(), bitmap.image.getHeight());

		constexpr static bool IsRLE4 = N == 4;
		constexpr static u32 directIndexBytes = IsRLE4 ? 0x80 : 0xFF;


		while (!eob) {

			if (reader.remainingSize() < 2) {
				throw ImageDecoderException("Stream size too small");
			}

			reader.read(std::span<u8>{ctrl, 2});

			if (ctrl[0]) {

				//n times index
				for (u32 i = 0; i < ctrl[0]; i++) {

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
						if (reader.remainingSize() < 2) {
							throw ImageDecoderException("Stream size too small");
						}

						reader.read(std::span<u8>{ctrl, 2});

						for (u32 i = 0; i < ctrl[0]; i++) {
							image.setPixel(x++, y, palette[skipColorIdx]);
						}

						for (u32 i = 0; i < ctrl[1]; i++) {

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

						if (reader.remainingSize() < byteCount) {
							throw ImageDecoderException("Stream size too small");
						}

						reader.read(std::span<u8>{indices, byteCount});

						if (byteCount & 1) {

							if (reader.remainingSize() < 1) {
								throw ImageDecoderException("Stream size too small");
							}

							reader.seek(1);

						}

						for (u32 i = 0; i < pixelCount; i++) {

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

		bitmap.image = image.makeRaw();

	};

	if (bitmap.compression == Bitmap::Compression::RLE4) {
		loadData.template operator()<4>();
	} else {
		loadData.template operator()<8>();
	}

}



void BitmapDecoder::decodeMasked() {

	u32 width = bitmap.image.getWidth();
	u32 height = bitmap.image.getHeight();

	u32 redMask = bitmap.redMask;
	u32 greenMask = bitmap.greenMask;
	u32 blueMask = bitmap.blueMask;
	u32 alphaMask = bitmap.alphaMask;

	u32 redShift = Bits::ctz(redMask);
	u32 greenShift = Bits::ctz(greenMask);
	u32 blueShift = Bits::ctz(blueMask);
	u32 alphaShift = Bits::ctz(alphaMask);

	u32 rowBytes = width * bitmap.bitsPerPixel / 8;
	u32 rowBytesAligned = Math::alignUp(rowBytes, 4);
	bool rowAlign = rowBytesAligned - rowBytes;

	if (reader.remainingSize() < rowBytesAligned * height) {
		throw ImageDecoderException("Stream size too small");
	}

	auto loadData = [&]<u32 N>() {

		using T = TT::Conditional<N != 16, u32, u16>;

		Image<Pixel::BGRA8> image(width, height);


		for (u32 y = 0; y < height; y++) {

			u32 ry = bitmap.topDown ? height - y - 1 : y;

			for (u32 x = 0; x < width; x++) {

				T pixelData = reader.read<T>();
				auto pixel = PixelConverter::convert<Pixel::BGRA8, T>(pixelData, redMask, redShift, greenMask, greenShift, blueMask, blueShift, alphaMask, alphaShift);
				image.setPixel(x, ry, pixel);

			}

			if constexpr (N == 16) {

				if (rowAlign) {
					reader.seek(2);
				}

			}

		}

		bitmap.image = image.makeRaw();

	};

	if (bitmap.bitsPerPixel == 16) {
		loadData.template operator()<16>();
	} else {
		loadData.template operator()<32>();
	}

}