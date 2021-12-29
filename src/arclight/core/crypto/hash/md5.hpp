/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 md5.hpp
 */

#pragma once

#include "hash.hpp"
#include "types.hpp"

#include <span>



namespace MD5 {

    Hash<128> hash(const std::span<const u8>& data) noexcept;

}