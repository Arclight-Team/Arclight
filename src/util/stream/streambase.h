#pragma once

#include "types.h"


class StreamBase {

public:

    enum class SeekMode {
		Begin,
		Current,
		End
	};

	virtual u64 seek(i64 offset, SeekMode mode = SeekMode::Begin) = 0;
	virtual u64 getPosition() const = 0;

	virtual u64 getSize() const = 0;
	virtual bool isOpen() const = 0;

};