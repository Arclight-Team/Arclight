/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 shaderstoragebuffer.cpp
 */

#include "shaderstoragebuffer.hpp"

#include "glecore.hpp"
#include GLE_HEADER


GLE_BEGIN


bool ShaderStorageBuffer::bindRange(u32 index, SizeT offset, SizeT size) {

	gle_assert(isBound(), "Shader storage buffer object %d has not been bound (attempted to set uniform range binding)", id);

	if (index >= Limits::getMaxShaderStorageBlockBindings()) {
		GLE::warn("Given storage block binding index %d exceeds the maximum of %d (shader storage buffer ID=%d)", index, Limits::getMaxShaderStorageBlockBindings(), id);
		return false;
	}

	if ((offset + size) > this->size) {
		GLE::warn("Storage block range to bind (offset = %d, %d bytes) exceeds the buffer size of %d (shader storage buffer ID=%d)", offset, size, this->size, id);
		return false;
	}

	glBindBufferRange(glBufferTypeEnum[u32(type)], index, id, offset, size);
	setBoundBufferID(type, id);

	return true;

}


GLE_END