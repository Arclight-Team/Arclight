#pragma once

#include "../gc.h"


GLE_BEGIN

enum class BufferTarget {
	VertexBuffer,
	ElementBuffer,
	TransformFeedbackBuffer,
	UniformBuffer,
	CopyReadBuffer,
	CopyWriteBuffer
};


enum class BufferAccess {
	StaticDraw,
	DynamicDraw,
	StreamDraw,
	StaticRead,
	DynamicRead,
	StreamRead,
	StaticCopy,
	DynamicCopy,
	StreamCopy
};


class Buffer {

public:

	constexpr Buffer() : id(invalidBufferID), target(BufferTarget::VertexBuffer), size(-1) {}

	//Creates a buffer if none has been created yet
	void create();

	//Binds the buffer to the given target if not already. Fails if it hasn't been created yet.
	void bind(BufferTarget target);

	//Destroys a buffer if it was created once
	void destroy();

	//Sets the buffer's storage. Required for (re-)allocation.
	void setStorage(u32 size, BufferAccess access = BufferAccess::StaticDraw);
	void setStorage(u32 size, void* data, BufferAccess access = BufferAccess::StaticDraw);

	//Updates the buffer's data. Fails if no storage has been allocated first.
	void setData(u32 offset, u32 size, void* data);

	//Copies this buffer (or a portion of it) to destBuffer
	void copy(Buffer& destBuffer);
	void copy(Buffer& destBuffer, u32 srcOffset, u32 destOffset, u32 size);

	//Checks the given states
	bool isCreated() const;
	bool isBound() const;

private:

	constexpr u32 getBoundBufferID(BufferTarget target) const {
		return boundBufferIDs[static_cast<u32>(target)];
	}

	static u32 getBufferTargetEnum(BufferTarget target);
	static u32 getBufferAccessEnum(BufferAccess access);


	u32 id;						//Buffer ID
	BufferTarget target;		//Current bound target
	u32 size;					//Buffer size or -1 if none has been allocated

	//Invalid IDs
	constexpr static inline u32 invalidBufferID = -1;
	constexpr static inline u32 invalidBoundID = -2;

	//Active buffer handles per type
	static inline u32 boundBufferIDs[6] = { invalidBoundID };

};

GLE_END