#pragma once

#include "stream.h"
#include "bytestreamimpl.h"
#include <span>


template<bool Dynamic>
class ByteStreamImplDRW : public Stream, private ByteStreamImplRW<Dynamic>
{
private:

	using MyBase = ByteStreamImplRW<Dynamic>;

public:

    using StreamBase::SeekMode;

	ByteStreamImplDRW() requires(Dynamic) = default;

    template<class T> requires Equal<T, std::remove_cv_t<T>>
    ByteStreamImplDRW(const std::span<T>& data) : MyBase(data) {}

	virtual SizeT read(void* dest, SizeT size) override {
		return MyBase::read(dest, size);
	}
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
		return MyBase::data;
	}
	constexpr auto getData() const {
		return MyBase::data;
	}

};

using ByteStream = ByteStreamImplDRW<false>;
using DynamicByteStream = ByteStreamImplDRW<true>;
