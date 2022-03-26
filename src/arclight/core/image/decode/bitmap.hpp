/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bitmap.hpp
 */

#pragma once

#include "types.hpp"
#include "image/rawimage.hpp"
#include "math/vector.hpp"



class Bitmap {

public:

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

	enum class ColorSpace {
		Calibrated = 0,
		SRGB = 0x73524742,
		SystemDefault = 0x57696E20,
		LinkedProfile = 0x4C494E4B,
		EmbeddedProfile = 0x4D424544
	};

	enum class RenderIntent {
		None = 0,
		Saturation = 1,
		RelativeColorimetric = 2,
		Perceptual = 4,
		AbsoluteColorimetric = 8
	};

	constexpr Bitmap() : version(Version::Info), compression(Compression::None), topDown(false), bitsPerPixel(24), pixelsPerMeterX(0), pixelsPerMeterY(0),
							paletteColors(0), importantColors(0), redMask(0), greenMask(0), blueMask(0), alphaMask(0), colorSpace(ColorSpace::Calibrated),
							redGamma(0), greenGamma(0), blueGamma(0), renderIntent(RenderIntent::None) {}

	Version version;
	Compression compression;

	bool topDown;
	u32 bitsPerPixel;
	i32 pixelsPerMeterX;
	i32 pixelsPerMeterY;

	u32 paletteColors;
	u32 importantColors;

	//V4 fields
	u32 redMask;
	u32 greenMask;
	u32 blueMask;
	u32 alphaMask;
	ColorSpace colorSpace;
	Vec3ui endpoint0;  //Color space point in CIE coordinates (Q2.30 fixed-point)
	Vec3ui endpoint1;
	Vec3ui endpoint2;
	u32 redGamma;       //Gamma in Q16.15
	u32 greenGamma;
	u32 blueGamma;

	//V5 fields
	RenderIntent renderIntent;

	//Embedded/Linked profiles are unsupported

	RawImage image;

};