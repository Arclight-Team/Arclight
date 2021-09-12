#pragma once

#include "stream.h"
#include <span>


class ByteReader : public Stream
{
public:

	ByteReader(const u8* data, u64 size);

	template<class T>
	ByteReader(const std::span<const T>& data) : position(0) {

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
	std::span<const std::byte> memory;

};