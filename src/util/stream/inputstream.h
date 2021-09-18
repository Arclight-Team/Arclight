#pragma once

#include "streambase.h"
#include <span>


class InputStream : virtual public StreamBase {

public:

    template<class T>
	SizeT read(const std::span<T>& dest) {
		read(dest.data(), dest.size());
	}

	virtual SizeT read(void* dest, SizeT size) = 0;

};