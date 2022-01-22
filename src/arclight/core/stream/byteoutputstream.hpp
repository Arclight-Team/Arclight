/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 byteoutputstream.hpp
 */

#pragma once

#include "outputstream.hpp"
#include "bytestreamimpl.hpp"
#include "util/typetraits.hpp"

#include <span>


template<bool Dynamic>
class ByteOutputStreamImplDRW : public OutputStream, private ByteStreamImplRW<Dynamic>
{
private:

	using MyBase = ByteStreamImplRW<Dynamic>;

public:

    using StreamBase::SeekMode;

	ByteOutputStreamImplDRW() requires(Dynamic) = default;

    template<class T> requires Equal<T, TT::RemoveCV<T>>
	ByteOutputStreamImplDRW(const std::span<T>& data) : MyBase(data) {}

	virtual SizeT write(const void* src, SizeT size) override {
		return MyBase::write(src, size);
	}

	virtual SizeT seek(i64 offset, SeekMode mode) override {
		return MyBase::seek(offset, mode);
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

	constexpr auto getData() {
		return MyBase::getData();
	}
	constexpr auto getData() const {
		return MyBase::getData();
	}


};

using ByteOutputStream = ByteOutputStreamImplDRW<false>;
using DynamicByteOutputStream = ByteOutputStreamImplDRW<true>;
