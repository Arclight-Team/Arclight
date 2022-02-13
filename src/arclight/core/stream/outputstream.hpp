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
  
	template<class T, SizeT Extent = std::dynamic_extent>
	SizeT write(const std::span<const T, Extent>& dest) {
		return write(dest.data(), dest.size_bytes());
	}

	virtual SizeT write(const void* src, SizeT size) = 0;

};