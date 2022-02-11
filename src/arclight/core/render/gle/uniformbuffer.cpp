/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 uniformbuffer.cpp
 */

#include "uniformbuffer.hpp"

#include "glecore.hpp"
#include GLE_HEADER


GLE_BEGIN


bool UniformBuffer::bindRange(u32 index, SizeT offset, SizeT size) {

	gle_assert(isBound(), "Uniform buffer object %d has not been bound (attempted to set uniform range binding)", id);

	if (index >= Limits::getMaxUniformBlockBindings()) {
		GLE::warn("Given uniform block binding index %d exceeds the maximum of %d (uniform buffer ID=%d)", index, Limits::getMaxUniformBlockBindings(), id);
		return false;
	}

	if ((offset + size) > this->size) {
		GLE::warn("Uniform range to bind (offset = %d, %d bytes) exceeds the buffer size of %d (uniform buffer ID=%d)", offset, size, this->size, id);
		return false;
	}

	glBindBufferRange(static_cast<u32>(type), index, id, offset, size);
	setBoundBufferID(type, id);

	return true;

}


GLE_END