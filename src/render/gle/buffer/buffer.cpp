#include "buffer.h"

#include GLE_HEADER


GLE_BEGIN


void Buffer::create() {

	if (!isCreated()) {
		glGenBuffers(1, &id);
	}

}



void Buffer::bind(BufferTarget target) {

	gle_assert(isCreated(), "Buffer object hasn't been created yet");

	this->target = target;

	if (!isBound()) {
		glBindBuffer(getBufferTargetEnum(target), id);
	}

}



void Buffer::destroy() {
	
	if (isCreated()) {

		glDeleteBuffers(1, &id);
		id = invalidBufferID;
		size = -1;

	}

}



void Buffer::setStorage(u32 size, BufferAccess access) {
	setStorage(size, nullptr, access);
}



void Buffer::setStorage(u32 size, void* data, BufferAccess access) {

	gle_assert(isBound(), "Buffer object %d has not been bound (attempted to set buffer storage)", id);

	//TODO: Check if buffer has been mapped non-persistently
	this->size = size;
	glBufferData(getBufferTargetEnum(target), size, data, getBufferAccessEnum(access));

}



void Buffer::setData(u32 offset, u32 size, void* data) {

	gle_assert(isBound(), "Buffer object %d has not been bound (attempted to set buffer data)", id);
	gle_assert((offset + size) <= this->size, "Attempted to write data out of bounds for buffer object %d", id);

	glBufferSubData(getBufferTargetEnum(target), offset, size, data);

}



void Buffer::copy(Buffer& destBuffer) {
	copy(destBuffer, 0, 0, this->size);
}



void Buffer::copy(Buffer& destBuffer, u32 srcOffset, u32 destOffset, u32 size) {

	gle_assert(isCreated(), "Buffer object %d hasn't been created yet", id);
	gle_assert((srcOffset + size) <= this->size, "Attempted to read data out of bounds for buffer object %d", id);
	gle_assert((destOffset + size) <= destBuffer.size, "Attempted to write data out of bounds for buffer object %d", destBuffer.id);

	//Backup old targets
	BufferTarget srcTarget = this->target;
	BufferTarget destTarget = destBuffer.target;

	//Bind to copy targets
	bind(BufferTarget::CopyReadBuffer);
	destBuffer.bind(BufferTarget::CopyWriteBuffer);

	//Now copy
	glCopyBufferSubData(getBufferTargetEnum(BufferTarget::CopyReadBuffer), getBufferTargetEnum(BufferTarget::CopyWriteBuffer), srcOffset, destOffset, size);

	//Restore old targets
	this->target = srcTarget;
	destBuffer.target = destTarget;

}



bool Buffer::isCreated() const {
	return id != invalidBufferID;
}



bool Buffer::isBound() const {
	return id == getBoundBufferID(target);
}



u32 Buffer::getBufferTargetEnum(BufferTarget target) {

	switch (target) {

		case BufferTarget::VertexBuffer:
			return GL_ARRAY_BUFFER;

		case BufferTarget::ElementBuffer:
			return GL_ELEMENT_ARRAY_BUFFER;

		case BufferTarget::TransformFeedbackBuffer:
			return GL_TRANSFORM_FEEDBACK_BUFFER;

		case BufferTarget::UniformBuffer:
			return GL_UNIFORM_BUFFER;

		case BufferTarget::CopyReadBuffer:
			return GL_COPY_READ_BUFFER;

		case BufferTarget::CopyWriteBuffer:
			return GL_COPY_WRITE_BUFFER;

		default:
			gle_assert(false, "Invalid buffer type 0x%X", target);
			return -1;

	}

}



u32 Buffer::getBufferAccessEnum(BufferAccess access) {

	switch (access) {

		case BufferAccess::StaticDraw:
			return GL_STATIC_DRAW;

		case BufferAccess::DynamicDraw:
			return GL_DYNAMIC_DRAW;

		case BufferAccess::StreamDraw:
			return GL_STREAM_DRAW;

		case BufferAccess::StaticRead:
			return GL_STATIC_READ;

		case BufferAccess::DynamicRead:
			return GL_DYNAMIC_READ;

		case BufferAccess::StreamRead:
			return GL_STREAM_READ;

		case BufferAccess::StaticCopy:
			return GL_STATIC_COPY;

		case BufferAccess::DynamicCopy:
			return GL_DYNAMIC_COPY;

		case BufferAccess::StreamCopy:
			return GL_STREAM_COPY;

		default:
			gle_assert(false, "Invalid buffer access mode 0x%X", access);
			return -1;

	}

}


GLE_END