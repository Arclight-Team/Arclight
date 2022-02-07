/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 horizontalheader.cpp
 */

#include "font/truetype/truetype.hpp"
#include "stream/binaryreader.hpp"
#include "math/math.hpp"
#include "util/bool.hpp"


namespace TrueType {

	HorizontalHeader parseHorizontalHeaderTable(BinaryReader& reader, u32 tableSize) {   

		if(tableSize < 36) {
			throw LoaderException("Failed to load horizontal header table: Stream size too small");
		}

		u32 version = reader.read<u32>();
		i16 ascent = reader.read<i16>();
		i16 descent = reader.read<i16>();
		i16 lineGap = reader.read<i16>();
		u16 advanceWidthMax = reader.read<u16>();
		i16 minLeftSideBearing = reader.read<i16>();
		i16 minRightSideBearing = reader.read<i16>();
		i16 xMaxExtent = reader.read<i16>();
		i16 caretSlopeRise = reader.read<i16>();
		i16 caretSlopeRun = reader.read<i16>();
		i16 caretOffset = reader.read<i16>();
		i16 reserved0 = reader.read<i16>();
		i16 reserved1 = reader.read<i16>();
		i16 reserved2 = reader.read<i16>();
		i16 reserved3 = reader.read<i16>();
		i16 metricDataFormat = reader.read<i16>();
		u16 numOfLongHorMetrics = reader.read<u16>();

#ifdef ARC_FONT_DEBUG
		Log::info("TrueType Loader", "[Horizontal Header] Version 0x%08X, Ascent: %d, Descent: %d, LineGap: %d, AdvanceWidthMax: %d, MinLeftSideBearing: %d, MinRightSideBearing: %d, XMaxExtent: %d",
					version, ascent, descent, lineGap, advanceWidthMax, minLeftSideBearing, minRightSideBearing, xMaxExtent);
		Log::info("TrueType Loader", "[Horizontal Header] CaretSlopeRise: %d, CaretSlopeRun: %d, CaretOffset: %d, Reserved0: %d, Reserved1: %d, Reserved2: %d, Reserved3: %d",
					caretSlopeRise, caretSlopeRun, caretOffset, reserved0, reserved1, reserved2, reserved3);
		Log::info("TrueType Loader", "[Horizontal Header] MetricDataFormat: %d, NumOfLongHorMetrics: %d", metricDataFormat, numOfLongHorMetrics);  
#endif

		if(version != 0x00010000) {
			throw LoaderException("Cannot read horizontal header table with versions newer than 1.0");
		}

		if(!caretSlopeRise && !caretSlopeRun) {
			throw LoaderException("Illegal caret parameters");
		}

		if(!Bool::all(0, reserved0, reserved1, reserved2, reserved3)) {
			throw LoaderException("Reserved fields must be 0");
		}

		if(metricDataFormat != 0) {
			throw LoaderException("Metric data format must be 0");
		}

		HorizontalHeader header;
		header.ascent = ascent;
		header.descent = descent;
		header.lineGap = lineGap;
		header.maxAdvance = advanceWidthMax;
		header.minLeftBearing = minLeftSideBearing;
		header.minRightBearing = minRightSideBearing;
		header.maxExtent = xMaxExtent;

		if (caretSlopeRun == 0) {
			header.caretAngle = Math::pi / 2;
		} else if (caretSlopeRise == 0) {
			header.caretAngle = 0;
		} else {
			header.caretAngle = Math::atan2(caretSlopeRise, caretSlopeRun);
		}

		header.caretOffset = caretOffset;
		header.metricsCount = numOfLongHorMetrics;

		return header;

	}

}