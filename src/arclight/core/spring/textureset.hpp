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

	bool isAdded(Id32 id) const;

	bool add(Id32 id, const Path& path);
	bool add(const std::vector<std::pair<Id32, Path>>& files);

	void clear();

	const std::unordered_map<u32, Path>& getTexturePaths() const;

private:

	std::unordered_map<u32, Path> texturePaths;

};