/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 textureset.cpp
 */

#include "textureset.hpp"
#include "filesystem/file.hpp"
#include "image/bmp.hpp"
#include "stream/fileinputstream.hpp"



bool TextureSet::isLoaded(Id64 id) const {
	return textures.contains(id);
}



bool TextureSet::load(Id64 id, const Path& path) {

	if (isLoaded(id)) {

		Log::error("Texture Set", "Texture with ID = %d already loaded", id);
		return false;

	}

	File file(path.getFilename(), File::In | File::Binary);

	if (!file.open()) {

		Log::error("Texture Set", "Failed to open file %s", path.getFilename().c_str());
		return false;

	}

	FileInputStream stream(file);
	textures.emplace(id, BMP::loadBitmap<Pixel::RGBA8>(stream));

	return true;

}



bool TextureSet::load(const std::vector<std::pair<Id64, Path>>& files) {

	for (const auto& [id, path] : files) {

		if (!load(id, path)) {
			return false;
		}

	}

	return true;

}



void TextureSet::clear() {
	textures.clear();
}



std::unordered_map<u64, Image<Pixel::RGBA8>> TextureSet::getAndClear() {
	return std::exchange(textures, {});
}
