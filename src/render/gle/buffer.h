#pragma once

#include "gc.h"
#include "globject.h"


GLE_BEGIN

enum class BufferType {
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


class Buffer : public GLObject {

public:

	//Creates a buffer if none has been created yet
	virtual bool create() override;

	//Destroys a buffer if it was created once
	virtual void destroy() override;

	//Allocates the buffer's storage. Required for (re-)allocation.
	void allocate(u32 size, BufferAccess access = BufferAccess::StaticDraw);
	void allocate(u32 size, void* data, BufferAccess access = BufferAccess::StaticDraw);

	//Updates the buffer's data. Fails if no storage has been allocated first.
	void update(u32 offset, u32 size, void* data);

	//Copies this buffer (or a portion of it) to destBuffer
	void copy(Buffer& destBuffer);
	void copy(Buffer& destBuffer, u32 srcOffset, u32 destOffset, u32 size);

	//Checks the given states
	bool isBound() const;
	bool isInitialized() const;

protected:

	//Yes, protected.
	constexpr Buffer(BufferType type) : type(type), size(0) {}

	//Binds the buffer to the given target if not already. Fails if it hasn't been created yet.
	void bind(BufferType type);

private:

	constexpr void setBoundBufferID(BufferType type, u32 id) const {
		boundBufferIDs[static_cast<u32>(type)] = id;
	}

	constexpr u32 getBoundBufferID(BufferType type) const {
		return boundBufferIDs[static_cast<u32>(type)];
	}

	static u32 getBufferTypeEnum(BufferType type);
	static u32 getBufferAccessEnum(BufferAccess access);

	BufferType type;			//Currently bound type
	u32 size;					//Buffer size or -1 if none has been allocated

	//Active buffer handles per type
	static inline u32 boundBufferIDs[6] = { invalidBoundID, invalidBoundID, invalidBoundID, invalidBoundID, invalidBoundID, invalidBoundID };

};

GLE_END