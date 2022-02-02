/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bytestream.hpp
 */

#pragma once

#include "stream.hpp"
#include "bytestreamimpl.hpp"
#include "util/typetraits.hpp"

#include <span>


template<bool Dynamic>
class ByteStreamImplDRW : public Stream, private ByteStreamImplRW<Dynamic>
{
private:

	using MyBase = ByteStreamImplRW<Dynamic>;

public:

	ByteStreamImplDRW() requires(Dynamic) = default;

	template<class T> requires Equal<T, TT::RemoveCV<T>>
	ByteStreamImplDRW(const std::span<T>& data) : MyBase(data) {}

	virtual SizeT read(void* dest, SizeT size) override {
		return MyBase::read(dest, size);
	}
	virtual SizeT write(const void* src, SizeT size) override {
		return MyBase::write(src, size);
	}

	virtual void seek(i64 offset) override {
		return MyBase::seek(offset);
	}

	virtual void seekTo(u64 offset) override {
		return MyBase::seekTo(offset);
	}

	virtual SizeT getPosition() const override {
		return MyBase::getPosition();
	}

	virtual SizeT getSize() const override {
		return MyBase::getSize();
	}
	virtual bool isOpen() const override {
		return MyBase::isOpen();
	}
	virtual bool reachedEnd() const override {
		return MyBase::reachedEnd();
	}

	inline auto getData() {
		return MyBase::data;
	}

	inline auto getData() const {
		return MyBase::data;
	}

};

using ByteStream = ByteStreamImplDRW<false>;
using DynamicByteStream = ByteStreamImplDRW<true>;
