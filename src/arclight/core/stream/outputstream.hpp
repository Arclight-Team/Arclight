#pragma once

#include "streambase.hpp"

#include <span>


class OutputStream : virtual public StreamBase {

public:
  
	template<class T>
	SizeT write(const std::span<const T>& dest) {
		return write(dest.data(), dest.size());
	}

	virtual SizeT write(const void* src, SizeT size) = 0;

};