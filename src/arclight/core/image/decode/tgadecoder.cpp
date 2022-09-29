/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 tgadecoder.cpp
 */

#include "tgadecoder.hpp"
#include "util/bool.hpp"
#include "util/string.hpp"
#include "common/exception.hpp"



enum class TGAImageType : u8
{
	None,
	ColorMap,
	TrueColor,
	BlackWhite,
	ColorMapRLE = 9,
	TrueColorRLE,
	BlackWhiteRLE
	/*
	*	TODO: look for more info about the two formats mentioned here
	*	http://www.paulbourke.net/dataformats/tga/
	*/
};

struct TGAColorMapSpecification {

	u16 firstEntryIndex;
	u16 colorMapLength;
	u8 colorMapEntrySize;

};

struct TGAImageSpecification {
	
	i16 originX; // Signed, according to
	i16 originY; // http://www.paulbourke.net/dataformats/tga/
	u16 width;
	u16 height;
	u8 pixelDepth;
	u8 imageDescriptor;

};

struct TGAHeader {

	u8 idLength;
	u8 colorMapType;
	TGAImageType imageType;
	TGAColorMapSpecification colorMapSpec;
	TGAImageSpecification imageSpec;

};



constexpr bool getValidImageType(const TGAHeader& hdr) {

	return Bool::any(
		hdr.imageType,
		TGAImageType::None,
		TGAImageType::ColorMap,
		TGAImageType::TrueColor,
		TGAImageType::BlackWhite,
		TGAImageType::ColorMapRLE,
		TGAImageType::TrueColorRLE,
		TGAImageType::BlackWhiteRLE);

}

constexpr bool getImageDataUncompressed(const TGAHeader& hdr) {
	return Bool::any(hdr.imageType, TGAImageType::ColorMap, TGAImageType::TrueColor, TGAImageType::BlackWhite);
}

constexpr bool getImageDataRLECompressed(const TGAHeader& hdr) {
	return Bool::any(hdr.imageType, TGAImageType::ColorMapRLE, TGAImageType::TrueColorRLE, TGAImageType::BlackWhiteRLE);
}

constexpr u32 getColorMapSize(const TGAColorMapSpecification& spec) {
	return spec.colorMapLength * spec.colorMapEntrySize / 8;
}

constexpr u8 getImageAlphaBits(const TGAImageSpecification& spec) {
	return Bits::mask(spec.imageDescriptor, 0, 4);
}

constexpr u8 getImageOriginMode(const TGAImageSpecification& spec) {
	return Bits::mask(spec.imageDescriptor, 4, 2);
}

constexpr u8 getImageReservedBits(const TGAImageSpecification& spec) {
	return Bits::mask(spec.imageDescriptor, 6, 2);
}

constexpr u32 getImageDataSize(const TGAImageSpecification& spec) {
	return spec.width * spec.height * spec.pixelDepth / 8;
}

constexpr bool getSupportedPixelDepth(const TGAImageSpecification& spec) {
	return Bool::any(spec.pixelDepth, 8, 15, 16, 24, 32);
}

constexpr u32 getTransformedX(const TGAImageSpecification& spec, u32 x) {
	return (getImageOriginMode(spec) & 0x10) ? spec.width - x - 1 : x;
}

constexpr u32 getTransformedY(const TGAImageSpecification& spec, u32 y) {
	return (getImageOriginMode(spec) & 0x20) ? y : spec.height - y - 1;
}



void TGADecoder::decode(std::span<const u8> data) {

	validDecode = false;

	id.clear();

	reader = BinaryReader(data);

	TGAHeader hdr{};

	// Read header
	hdr.idLength = reader.read<u8>();

	hdr.colorMapType = reader.read<u8>();

	if (hdr.colorMapType > 1)
		throw ImageDecoderException("Invalid color map type");

	hdr.imageType = TGAImageType(reader.read<u8>());

	if (!getValidImageType(hdr))
		throw ImageDecoderException("Invalid image type");

	// Read color map specification
	if (hdr.colorMapType == 1) {

		hdr.colorMapSpec.firstEntryIndex = reader.read<u16>();
		hdr.colorMapSpec.colorMapLength = reader.read<u16>();
		hdr.colorMapSpec.colorMapEntrySize = reader.read<u8>();

	} else {
		reader.seek(5);
	}

	// Read image specification
	origin.x = hdr.imageSpec.originX = reader.read<i16>();
	origin.y = hdr.imageSpec.originY = reader.read<i16>();

	hdr.imageSpec.width = reader.read<u16>();

	if (hdr.imageSpec.width == 0)
		throw ImageDecoderException("Width should not be zero");

	hdr.imageSpec.height = reader.read<u16>();

	if (hdr.imageSpec.height == 0)
		throw ImageDecoderException("Width should not be zero");

	hdr.imageSpec.pixelDepth = reader.read<u8>();

	if (!getSupportedPixelDepth(hdr.imageSpec))
		throw ImageDecoderException("Invalid pixel depth");

	hdr.imageSpec.imageDescriptor = reader.read<u8>();

	if (getImageReservedBits(hdr.imageSpec))
		Log::warn("TGADecoder", "ImageDescriptor reserved bits are not zero");

	// Read image ID
	id.resize(hdr.idLength);

	if (hdr.idLength)
		reader.read<u8>(id);

	// Read color map data
	if (hdr.colorMapType == 1) {

		u32 colorMapSize = getColorMapSize(hdr.colorMapSpec);

		colorMapData.resize(colorMapSize);

		reader.read<u8>(colorMapData);

	}
	
	// Read image data
	if (hdr.imageType != TGAImageType::None) {

		u32 imageDataSize = getImageDataSize(hdr.imageSpec);

		imageData.resize(imageDataSize);

		if (getImageDataRLECompressed(hdr)) {
			readImageDataRLE(hdr);
		} else {
			reader.read<u8>(imageData);
		}

	}

	switch (hdr.imageType) {

	case TGAImageType::None:			// No image data
		image = Image<Pixel::BGRA8>(hdr.imageSpec.width, hdr.imageSpec.height).makeRaw();
		break;

	case TGAImageType::ColorMap:		// Uncompressed, Color mapped
	case TGAImageType::ColorMapRLE:		// Run-length encoded, Color mapped	
		parseColorMapImageData(hdr);
		break;

	case TGAImageType::TrueColor:		// Uncompressed, True color
	case TGAImageType::TrueColorRLE:	// Run-length encoded, True color
		parseTrueColorImageData(hdr);
		break;

		// TODO: untested
	case TGAImageType::BlackWhite:		// Uncompressed, Black and white
	case TGAImageType::BlackWhiteRLE:	// Run-length encoded, Black and white
		parseBlackWhiteImageData(hdr);
		break;

		throw UnsupportedOperationException("Invalid/unsupported TGA image type");

	}

	validDecode = true;

}



RawImage& TGADecoder::getImage() {

	if (!validDecode) {
		throw ImageDecoderException("Bad image decode");
	}

	return image;

}



void TGADecoder::readImageDataRLE(const TGAHeader& hdr) {

	u8 buffer[5]{};
	u32 pixelSize = hdr.imageSpec.pixelDepth / 8;
	u32 pixelCount = hdr.imageSpec.width * hdr.imageSpec.height;

	for (u32 p = 0; p < pixelCount * pixelSize;) {

		reader.read<u8>({ buffer,  pixelSize + 1 });

		// Insert reference pixel
		std::copy_n(buffer + 1, pixelSize, imageData.begin() + p);
		p += pixelSize;

		// RLE control byte
		u32 n = buffer[0] & 0x7F;

		if (buffer[0] & 0x80) {

			// Insert pixel n times
			for (u32 i = 0; i < n; i++) {

				std::copy_n(buffer + 1, pixelSize, imageData.begin() + p);
				p += pixelSize;

			}

		}
		else {

			// Read and insert n pixels
			reader.read<u8>({ imageData.begin() + p, n * pixelSize });
			p += n * pixelSize;

		}

	}

}



void TGADecoder::parseColorMapImageData(const TGAHeader& hdr) {

	using ColorsT = Colors<Pixel::BGRA8>;

	std::vector<PixelBGRA8> colorMap;

	auto convertColorMap = [&]<Pixel P, SizeT Size>() {

		auto colorMapView = std::span{ colorMapData };

		colorMap.resize(hdr.colorMapSpec.colorMapLength);

		bool alpha15 = hdr.imageSpec.pixelDepth == 16;

		for (u32 i = 0; i < hdr.colorMapSpec.colorMapLength; i++) {

			auto pixelData = colorMapView.subspan(i * Size, Size);

			if (alpha15 && (pixelData.back() & 0x80) == 0) {
				colorMap[i] = ColorsT::Transparent;
			}

			colorMap[i] = PixelConverter::convert<Pixel::BGRA8>(PixelType<P>(pixelData));

		}

	};

	auto buildImage = [&]<class T>() {

		Image<Pixel::BGRA8> bufImage(hdr.imageSpec.width, hdr.imageSpec.height);

		BinaryReader dataReader(imageData);

		bool alpha15 = hdr.imageSpec.pixelDepth == 16;

		for (u32 y = 0; y < hdr.imageSpec.height; y++) {

			// TODO: profile the usage of getter functions vs booleans for flipX/flipY
			u32 ry = getTransformedY(hdr.imageSpec, y);

			for (u32 x = 0; x < hdr.imageSpec.width; x++) {

				u32 rx = getTransformedX(hdr.imageSpec, x);

				T pixelData = dataReader.read<T>();

				// TODO: just a speculation, not actually described anywhere... requires more testing
				if (alpha15 && (pixelData & 0x8000) == 0) {
					bufImage.setPixel(rx, ry, ColorsT::Transparent);
				} else {
					pixelData &= 0x7FFF;
				}

				if (pixelData >= hdr.colorMapSpec.colorMapLength)
					throw ImageDecoderException("Invalid color map index found in image data");

				bufImage.setPixel(rx, ry, colorMap[pixelData]);

			}
		}

		image = bufImage.makeRaw();

	};

	// Convert color map to ARGB8
	switch (hdr.colorMapSpec.colorMapEntrySize) {

	case 15:
	case 16:
		convertColorMap.template operator()<Pixel::RGB5, 2>();
		break;

	case 24:
		convertColorMap.template operator()<Pixel::RGB8, 3>();
		break;

	case 32:
		convertColorMap.template operator()<Pixel::RGBA8, 4>();
		break;

	default:
		throw ImageDecoderException("Invalid color map color format");

	}

	switch (hdr.imageSpec.pixelDepth) {

	case 8:	
		buildImage.template operator()<u8>();
		break;

		/*
		*	Wikipedia mentions 15-bit index formats
		*	15th bit reserved for alpha/clear color?
		*	This format is an inconsistent mess
		*/

	case 15:
	case 16:
		buildImage.template operator()<u16>();
		break;

	case 24:
	case 32:
		throw UnsupportedOperationException(String::format("Color map TGA format with %d-bits indices is not supported", hdr.imageSpec.pixelDepth));

	default:
		throw ImageDecoderException("Invalid pixel format");

	}

}



void TGADecoder::parseTrueColorImageData(const TGAHeader& hdr) {

	using ColorsT = Colors<Pixel::BGRA8>;

	auto loadData = [&]<Pixel P, SizeT Size>() {

		BinaryReader reader(imageData);
		Image<Pixel::BGRA8> bufImage(hdr.imageSpec.width, hdr.imageSpec.height);

		bool alpha15 = hdr.imageSpec.pixelDepth == 16;

		for (u32 y = 0; y < hdr.imageSpec.height; y++) {

			// TODO: profile the usage of getter functions vs booleans for flipX/flipY
			u32 ry = getTransformedY(hdr.imageSpec, y);

			for (u32 x = 0; x < hdr.imageSpec.width; x++) {

				u32 rx = getTransformedX(hdr.imageSpec, x);

				u8 pixelData[Size];
				reader.read<u8>(pixelData);

				if (alpha15 && (pixelData[Size - 1] & 0x80) == 0) {
					bufImage.setPixel(rx, ry, ColorsT::Transparent);
				} else {

					auto pixel = PixelConverter::convert<Pixel::BGRA8>(PixelType<P>(pixelData));
					bufImage.setPixel(rx, ry, pixel);

				}

			}
		}

		image = bufImage.makeRaw();

	};

	switch (hdr.imageSpec.pixelDepth) {

	case 15:
	case 16:
		loadData.template operator()<Pixel::RGB5, 2>();
		break;

	case 24:
		loadData.template operator()<Pixel::RGB8, 3>();
		break;

	case 32:
		loadData.template operator()<Pixel::RGBA8, 4>();
		break;

	default:
		throw ImageDecoderException("Invalid pixel format");

	}

}



void TGADecoder::parseBlackWhiteImageData(const TGAHeader& hdr) {

	using ColorsT = Colors<Pixel::BGRA8>;

	auto loadData = [&]() {

		BinaryReader reader(imageData);
		Image<Pixel::BGRA8> bufImage(hdr.imageSpec.width, hdr.imageSpec.height);
		
		for (u32 y = 0; y < hdr.imageSpec.height; y++) {

			// TODO: profile the usage of getter functions vs booleans for flipX/flipY
			u32 ry = getTransformedY(hdr.imageSpec, y);

			for (u32 x = 0; x < hdr.imageSpec.width; x++) {

				u32 rx = getTransformedX(hdr.imageSpec, x);

				u8 pixelData = reader.read<u8>();

				auto pixel = PixelConverter::convert<Pixel::BGRA8>(PixelRGB8(pixelData, pixelData, pixelData));
				bufImage.setPixel(rx, ry, pixel);

			}
		}

		image = bufImage.makeRaw();

	};

	switch (hdr.imageSpec.pixelDepth) {

	case 8:
		loadData();
		break;

	default:
		throw ImageDecoderException("Invalid pixel format");

	}

}
