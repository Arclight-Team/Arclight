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

	template<class T>
	SizeT read(const std::span<T>& dest) {
		return read(dest.data(), dest.size());
	}

	virtual SizeT read(void* dest, SizeT size) = 0;

};