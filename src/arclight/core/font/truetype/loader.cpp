/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 loader.cpp
 */

#include "loader.hpp"
#include "truetype.hpp"
#include "stream/binaryreader.hpp"
#include "util/log.hpp"



namespace TrueType {

	Font loadFontInternal(BinaryReader& reader, TableMap& tables) {

		reader.seekTo(tables[TableType::Name].offset);
		parseNameTable(reader, tables[TableType::Name].length);

		reader.seekTo(tables[TableType::Header].offset);
		FontHeader fontHeader = parseHeaderTable(reader, tables[TableType::Header].length);

		reader.seekTo(tables[TableType::MaxProfile].offset);
		MaximumProfile maxp = parseMaxProfileTable(reader, tables[TableType::MaxProfile].length);

		reader.seekTo(tables[TableType::PostScript].offset);
		parsePostScriptTable(reader, tables[TableType::PostScript].length, maxp.glyphCount);

		reader.seekTo(tables[TableType::HorizontalHeader].offset);
		HorizontalHeader hhead = parseHorizontalHeaderTable(reader, tables[TableType::HorizontalHeader].length);

		reader.seekTo(tables[TableType::CharMap].offset);
		std::unordered_map<u32, u32> charMap = parseCharacterMapTable(reader, tables[TableType::CharMap].length);

		reader.seekTo(tables[TableType::Location].offset);
		std::vector<u32> glyphOffsets = parseGlyphLocationTable(reader, tables[TableType::Location].length, maxp.glyphCount, fontHeader.longLocationFormat);

		reader.seekTo(tables[TableType::GlyphData].offset);
		std::vector<Glyph> glyphs = parseGlyphTable(reader, tables[TableType::GlyphData].length, glyphOffsets);

		reader.seekTo(tables[TableType::HorizontalMetrics].offset);
		parseHorizontalMetricsTable(reader, tables[TableType::HorizontalMetrics].length, glyphs, hhead.metricsCount);

		return {fontHeader, charMap, glyphs};

	}

	Font loadFont(const std::span<const u8>& data) {

		try {

			BinaryReader reader(data, ByteOrder::Big);

			TableMap tables = parseFileHeader(reader);
			return loadFontInternal(reader, tables);            

		} catch (const LoaderException& e) {
			Log::error("TrueType Loader", e.what());
			return {};
		}

	}


	std::vector<Font> loadFontCollection(const std::span<const u8>& data) {

		try {

			BinaryReader reader(data, ByteOrder::Big);

			std::vector<TableMap> tableMaps = parseSharedFileHeader(reader);
			std::vector<Font> fonts;

			for(TableMap& tables : tableMaps) {
				fonts.emplace_back(loadFontInternal(reader, tables));
			}

			return fonts;

		} catch (const LoaderException& e) {
			Log::error("TrueType Loader", e.what());
			return {};
		}

	}


}