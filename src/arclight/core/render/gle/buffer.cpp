#include "buffer.hpp"

#include GLE_HEADER


GLE_BEGIN


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
		glBindBuffer(getBufferTypeEnum(type), id);
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

	}

}



void Buffer::allocate(u32 size, BufferAccess access) {
	allocate(size, nullptr, access);
}



void Buffer::allocate(u32 size, const void* data, BufferAccess access) {

	gle_assert(isBound(), "Buffer object %d has not been bound (attempted to set buffer storage)", id);

	//TODO: Check if buffer has been mapped non-persistently
	this->size = size;
	glBufferData(getBufferTypeEnum(type), size, data, getBufferAccessEnum(access));

}



void Buffer::update(u32 offset, u32 size, const void* data) {

	gle_assert(isBound(), "Buffer object %d has not been bound (attempted to set buffer data)", id);
	gle_assert((offset + size) <= this->size, "Attempted to write data out of bounds for buffer object %d", id);

	glBufferSubData(getBufferTypeEnum(type), offset, size, data);

}



void Buffer::copy(Buffer& destBuffer) {
	copy(destBuffer, 0, 0, this->size);
}



void Buffer::copy(Buffer& destBuffer, u32 srcOffset, u32 destOffset, u32 size) {

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
	glCopyBufferSubData(getBufferTypeEnum(BufferType::CopyReadBuffer), getBufferTypeEnum(BufferType::CopyWriteBuffer), srcOffset, destOffset, size);

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



u32 Buffer::getSize() const {
	return size;
}



u32 Buffer::getBufferTypeEnum(BufferType type) {

	switch (type) {

		case BufferType::VertexBuffer:
			return GL_ARRAY_BUFFER;

		case BufferType::ElementBuffer:
			return GL_ELEMENT_ARRAY_BUFFER;

		case BufferType::TransformFeedbackBuffer:
			return GL_TRANSFORM_FEEDBACK_BUFFER;

		case BufferType::UniformBuffer:
			return GL_UNIFORM_BUFFER;

		case BufferType::CopyReadBuffer:
			return GL_COPY_READ_BUFFER;

		case BufferType::CopyWriteBuffer:
			return GL_COPY_WRITE_BUFFER;

		default:
			gle_force_assert("Invalid buffer type 0x%X", type);
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
			gle_force_assert("Invalid buffer access mode 0x%X", access);
			return -1;

	}

}


GLE_END