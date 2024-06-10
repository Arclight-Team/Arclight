/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 TextureSet.hpp
 */

#pragma once

#include "Filesystem/Path.hpp"
#include "Image/Image.hpp"

#include <vector>



struct TextureLoadData {

	u32 id;
	Path path;
	bool hasAlpha;

};



class TextureSet {

public:

	bool isAdded(u32 id) const;

	bool add(u32 id, const Path& path, bool hasAlpha);
	bool add(const std::vector<TextureLoadData>& textures);

	void clear();

	const std::unordered_map<u32, TextureLoadData>& getTexturePaths() const;

private:

	std::unordered_map<u32, TextureLoadData> texturePaths;

};