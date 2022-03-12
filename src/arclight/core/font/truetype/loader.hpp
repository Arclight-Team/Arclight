/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 loader.hpp
 */

#pragma once

#include "truetype.hpp"

#include <span>



namespace TrueType {

	Font loadFont(const std::span<const u8>& data);
	std::vector<Font> loadFontCollection(const std::span<const u8>& data);

}