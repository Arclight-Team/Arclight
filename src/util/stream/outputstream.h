#pragma once

#include "streambase.h"
#include <span>


class OutputStream : virtual public StreamBase {

public:
  
	template<class T>
	u64 write(const std::span<const T>& dest) {
		write(dest.data(), dest.size());
	}

	virtual u64 write(const void* src, u64 size) = 0;

};