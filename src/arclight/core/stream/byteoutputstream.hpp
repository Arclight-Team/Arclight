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

	template<class T, SizeT Extent>
	ByteOutputStreamImplDRW(const std::span<T, Extent>& data) : MyBase(data) {}

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

	using MyBase::getData;

};

using ByteOutputStream = ByteOutputStreamImplDRW<false>;
using DynamicByteOutputStream = ByteOutputStreamImplDRW<true>;
