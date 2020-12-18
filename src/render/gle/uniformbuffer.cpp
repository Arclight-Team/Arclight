#include "uniformbuffer.h"

#include "glecore.h"
#include GLE_HEADER


GLE_BEGIN


bool UniformBuffer::bindRange(u32 index, u32 offset, u32 size) {

	gle_assert(isBound(), "Uniform buffer object %d has not been bound (attempted to set uniform range binding)", id);

	if (index >= Limits::getMaxUniformBlockBindings()) {
		GLE::warn("Given uniform block binding index %d exceeds the maximum of %d (uniform buffer ID=%d)", index, Limits::getMaxUniformBlockBindings(), id);
		return false;
	}

	if ((offset + size) > this->size) {
		GLE::warn("Uniform range to bind (offset = %d, %d bytes) exceeds the buffer size of %d (uniform buffer ID=%d)", offset, size, this->size, id);
		return false;
	}

	glBindBufferRange(getBufferTypeEnum(type), index, id, offset, size);
	setBoundBufferID(type, id);

}


GLE_END