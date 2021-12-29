/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 streambase.hpp
 */

#pragma once

#include "types.hpp"


class StreamBase {

public:

    enum class SeekMode {
		Begin,
		Current,
		End
	};

	virtual SizeT seek(i64 offset, SeekMode mode = SeekMode::Begin) = 0;
	virtual SizeT getPosition() const = 0;

	virtual SizeT getSize() const = 0;
	virtual bool isOpen() const = 0;
	virtual bool reachedEnd() const = 0;

};