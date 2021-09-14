#pragma once

#include "streambase.h"
#include <span>


class InputStream : virtual public StreamBase {

public:

    template<class T>
	u64 read(const std::span<T>& dest) {
		read(dest.data(), dest.size());
	}

	virtual u64 read(void* dest, u64 size) = 0;

};