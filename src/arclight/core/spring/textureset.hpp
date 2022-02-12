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



class TextureSet {

public:

	bool isLoaded(Id64 id) const;

	bool load(Id64 id, const Path& path);
	bool load(const std::vector<std::pair<Id64, Path>>& files);

	void clear();

	std::unordered_map<u64, Image<Pixel::RGBA8>> getAndClear();

private:

	std::unordered_map<u64, Image<Pixel::RGBA8>> textures;

};