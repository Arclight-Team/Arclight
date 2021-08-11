#pragma once
#include "types.h"
#include "util/bits.h"
#include "util/assert.h"

class Stream
{
public:

	enum class SeekMode
	{
		Begin,
		Current,
		End,
	};

	virtual u64 read(void* dest, u64 size) = 0;
	virtual u64 write(const void* src, u64 size) = 0;

	virtual u64 seek(i64 offset, SeekMode mode = SeekMode::Begin) = 0;
	virtual u64 tell() const = 0;

	virtual u64 getSize() const = 0;
	virtual bool isOpen() const = 0;

};