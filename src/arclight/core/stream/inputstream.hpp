#pragma once

#include "streambase.hpp"

#include <span>


class InputStream : virtual public StreamBase {

public:

    template<class T>
	SizeT read(const std::span<T>& dest) {
		return read(dest.data(), dest.size());
	}

	virtual SizeT read(void* dest, SizeT size) = 0;

};