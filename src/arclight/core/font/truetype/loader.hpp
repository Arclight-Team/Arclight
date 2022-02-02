/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 loader.hpp
 */

#pragma once

#include "truetype.hpp"


class InputStream;

namespace TrueType {

	Font loadFont(InputStream& stream);
	std::vector<Font> loadFontCollection(InputStream& stream);

}