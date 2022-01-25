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

	virtual void seek(i64 offset) = 0;
	virtual void seekTo(u64 offset) = 0;
	virtual SizeT getPosition() const = 0;

	virtual SizeT getSize() const = 0;
	virtual bool isOpen() const = 0;
	virtual bool reachedEnd() const = 0;

};