/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 inputstream.hpp
 */

#pragma once

#include "streambase.hpp"

#include <span>


class InputStream : virtual public StreamBase {

public:

	template<class T, SizeT Extent = std::dynamic_extent>
	SizeT read(const std::span<T, Extent>& dest) {
		return read(dest.data(), dest.size_bytes());
	}

	virtual SizeT read(void* dest, SizeT size) = 0;

};