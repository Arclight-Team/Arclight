/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 TextureSet.cpp
 */

#include "Spring/TextureSet.hpp"



bool TextureSet::isAdded(u32 id) const {
	return texturePaths.contains(id);
}



bool TextureSet::add(u32 id, const Path& path, bool hasAlpha) {

	if (isAdded(id)) {

		LogE("Spring").print("Texture with ID = %d already added", id);
		return false;

	}

	texturePaths.emplace(id, TextureLoadData {id, path, hasAlpha});

	return true;

}



bool TextureSet::add(const std::vector<TextureLoadData>& textures) {

	for (const auto& [id, path, alpha] : textures) {

		if (!add(id, path, alpha)) {
			return false;
		}

	}

	return true;

}



void TextureSet::clear() {
	texturePaths.clear();
}



const std::unordered_map<u32, TextureLoadData>& TextureSet::getTexturePaths() const {
	return texturePaths;
}
