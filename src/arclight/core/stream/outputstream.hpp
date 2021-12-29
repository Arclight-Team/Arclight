/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 outputstream.hpp
 */

#pragma once

#include "streambase.hpp"

#include <span>


class OutputStream : virtual public StreamBase {

public:
  
	template<class T>
	SizeT write(const std::span<const T>& dest) {
		return write(dest.data(), dest.size());
	}

	virtual SizeT write(const void* src, SizeT size) = 0;

};