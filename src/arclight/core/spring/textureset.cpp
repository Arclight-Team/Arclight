/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 textureset.cpp
 */

#include "textureset.hpp"



bool TextureSet::isAdded(Id32 id) const {
	return texturePaths.contains(id);
}



bool TextureSet::add(Id32 id, const Path& path) {

	if (isAdded(id)) {

		Log::error("Texture Set", "Texture with ID = %d already added", id);
		return false;

	}

	texturePaths.emplace(id, path);

	return true;

}



bool TextureSet::add(const std::vector<std::pair<Id32, Path>>& files) {

	for (const auto& [id, path] : files) {

		if (!add(id, path)) {
			return false;
		}

	}

	return true;

}



void TextureSet::clear() {
	texturePaths.clear();
}



const std::unordered_map<u32, Path>& TextureSet::getTexturePaths() const {
	return texturePaths;
}
