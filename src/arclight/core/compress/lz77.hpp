/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 lz77.hpp
 */

#pragma once

#include "types.hpp"

#include <vector>


class InputStream;

namespace LZ77 {

    std::vector<u8> compress(InputStream& stream);
    std::vector<u8> decompress(InputStream& stream);

}