#pragma once
#include "stream.h"
#include <vector>


class MemoryStream : public Stream
{
public:

	MemoryStream(const u8* data, u64 size);
	MemoryStream(const std::vector<u8>& data);
	template<class T>
	MemoryStream(const std::vector<T>& data) : position(0) {

		// convert T vector to u8 vector
		memory.resize(data.size() * sizeof(T));
		std::memcpy(memory.data(), data.data(), memory.size());

	}

	virtual u64 read(void* dest, u64 size) override;
	virtual u64 write(const void* src, u64 size) override;

	virtual u64 seek(i64 offset, SeekMode mode = SeekMode::Begin) override;
	virtual u64 tell() const override;

	virtual u64 getSize() const override;
	virtual bool isOpen() const override;

private:

	u64 position;
	std::vector<u8> memory;

};