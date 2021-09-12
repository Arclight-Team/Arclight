#pragma once

#include "stream.h"
#include <span>


class ByteWriter : public Stream
{
public:

	ByteWriter(u8* data, u64 size);

	template<class T>
	ByteWriter(const std::span<T>& data) : position(0) {

		//convert T span to u8 span
		memory = data.as_bytes();

	}

	virtual u64 read(void* dest, u64 size) override;
	virtual u64 write(const void* src, u64 size) override;

	virtual u64 seek(i64 offset, SeekMode mode = SeekMode::Begin) override;
	virtual u64 tell() const override;

	virtual u64 getSize() const override;
	virtual bool isOpen() const override;

private:

	u64 position;
	std::span<std::byte> memory;

};