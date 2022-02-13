/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 buffer.cpp
 */

#include "buffer.hpp"

#include GLE_HEADER


GLE_BEGIN


constexpr u32 glBufferTypeEnum[] = {
	0x8892, // GL_ARRAY_BUFFER
	0x8893,	// GL_ELEMENT_ARRAY_BUFFER
	0x8C8E,	// GL_TRANSFORM_FEEDBACK_BUFFER
	0x8A11,	// GL_UNIFORM_BUFFER
	0x8F36,	// GL_COPY_READ_BUFFER
	0x8F37,	// GL_COPY_WRITE_BUFFER
	0x90D2,	// GL_SHADER_STORAGE_BUFFER
	0x88EB,	// GL_PIXEL_PACK_BUFFER
	0x88EC,	// GL_PIXEL_UNPACK_BUFFER
	0x8C2A,	// GL_TEXTURE_BUFFER
};


bool Buffer::create() {

	if (!isCreated()) {

		glGenBuffers(1, &id);

		if (!id) {

			id = invalidID;
			return false;

		}

	}

	return true;

}



void Buffer::bind(BufferType type) {

	gle_assert(isCreated(), "Buffer object hasn't been created yet");

	this->type = type;

	if (!isBound()) {
		glBindBuffer(glBufferTypeEnum[u32(type)], id);
		setBoundBufferID(type, id);
	}

}



void Buffer::destroy() {
	
	if (isCreated()) {

		if (isBound()) {
			setBoundBufferID(type, invalidBoundID);
		}

		glDeleteBuffers(1, &id);
		id = invalidID;
		size = 0;
		mapped = false;

	}

}



void Buffer::allocate(SizeT size, BufferAccess access) {
	allocate(size, nullptr, access);
}



void Buffer::allocate(SizeT size, const void* data, BufferAccess access) {

	gle_assert(isBound(), "Buffer object %d has not been bound (attempted to set buffer storage)", id);

	//TODO: Check if buffer has been mapped non-persistently
	this->size = size;
	glBufferData(glBufferTypeEnum[u32(type)], size, data, static_cast<u32>(access));

}



void Buffer::update(SizeT offset, SizeT size, const void* data) {

	gle_assert(isBound(), "Buffer object %d has not been bound (attempted to set buffer data)", id);
	gle_assert((offset + size) <= this->size, "Attempted to write data out of bounds for buffer object %d", id);

	glBufferSubData(glBufferTypeEnum[u32(type)], offset, size, data);

}



void* Buffer::map(Access access) {

	gle_assert(isBound(), "Buffer object %d has not been bound (attempted to map buffer)", id);
	gle_assert(!isMapped(), "Attempted to map already mapped buffer object %d", id);

	mapped = true;

	return glMapBuffer(glBufferTypeEnum[u32(type)], static_cast<u32>(access));

}



void Buffer::unmap() {

	gle_assert(isBound(), "Buffer object %d has not been bound (attempted to unmap buffer)", id);
	gle_assert(isMapped(), "Attempted to unmap non mapped buffer object %d", id);

	glUnmapBuffer(glBufferTypeEnum[u32(type)]);

}



void Buffer::unbind() {

	gle_assert(isBound(), "Buffer object %d has not been bound (attempted to unbind buffer)", id);

	setBoundBufferID(type, invalidBoundID);

	glBindBuffer(glBufferTypeEnum[u32(type)], 0);

}



void Buffer::copy(Buffer& destBuffer) {
	copy(destBuffer, 0, 0, this->size);
}



void Buffer::copy(Buffer& destBuffer, SizeT srcOffset, SizeT destOffset, SizeT size) {

	gle_assert(isCreated(), "Buffer object %d hasn't been created yet", id);
	gle_assert((srcOffset + size) <= this->size, "Attempted to read data out of bounds for buffer object %d", id);
	gle_assert((destOffset + size) <= destBuffer.size, "Attempted to write data out of bounds for buffer object %d", destBuffer.id);

	//Backup old types
	BufferType srcType = this->type;
	BufferType destType = destBuffer.type;

	//Bind to copy targets
	bind(BufferType::CopyReadBuffer);
	destBuffer.bind(BufferType::CopyWriteBuffer);

	//Now copy
	glCopyBufferSubData(static_cast<u32>(BufferType::CopyReadBuffer), static_cast<u32>(BufferType::CopyWriteBuffer), srcOffset, destOffset, size);

	//Restore old targets
	this->type = srcType;
	destBuffer.type = destType;

}



bool Buffer::isBound() const {
	return id == getBoundBufferID(type);
}



bool Buffer::isInitialized() const {
	return size;
}



bool Buffer::isMapped() const {
	return mapped;
}



SizeT Buffer::getSize() const {
	return size;
}


GLE_END