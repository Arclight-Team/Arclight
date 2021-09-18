#pragma once

#include "types.h"


class StreamBase {

public:

    enum class SeekMode {
		Begin,
		Current,
		End
	};

	virtual SizeT seek(i64 offset, SeekMode mode = SeekMode::Begin) = 0;
	virtual SizeT getPosition() const = 0;

	virtual SizeT getSize() const = 0;
	virtual bool isOpen() const = 0;
	virtual bool reachedEnd() const = 0;

};