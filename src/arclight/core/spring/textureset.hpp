/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 textureset.hpp
 */

#pragma once

#include "filesystem/path.hpp"
#include "image/image.hpp"
#include "util/identifier.hpp"

#include <string>
#include <vector>



struct TextureLoadData {

	Id32 id;
	Path path;
	bool hasAlpha;

};



class TextureSet {

public:

	bool isAdded(Id32 id) const;

	bool add(Id32 id, const Path& path, bool hasAlpha);
	bool add(const std::vector<TextureLoadData>& textures);

	void clear();

	const std::unordered_map<u32, TextureLoadData>& getTexturePaths() const;

private:

	std::unordered_map<u32, TextureLoadData> texturePaths;

};