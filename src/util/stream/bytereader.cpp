#include "bytereader.h"
#include "util/math.h"
#include "util/assert.h"


ByteReader::ByteReader(const u8* data, u64 size) : memory(static_cast<const std::byte*>(static_cast<const void*>(data)), size), position(0) {}


u64 ByteReader::read(void* dest, u64 size) {

	arc_assert(dest != nullptr, "Destination is null");
	arc_assert(position + size <= getSize(), "Cannot read past the end of the stream");

	size = Math::min(size, getSize() - position);
	std::memcpy(dest, memory.data() + position, size);
	
	position += size;

	return size;

}

u64 ByteReader::write(const void* src, u64 size) {

	arc_force_assert("Cannot write with read-only stream");
	return 0;

}



u64 ByteReader::seek(i64 offset, SeekMode mode) {

	switch (mode) {

	case SeekMode::Begin:
		arc_assert(offset >= 0, "Cannot seek before the start of the stream");
		position = offset;
		break;

	case SeekMode::Current:
		position = tell() + offset;
		break;

	case SeekMode::End:
		arc_assert(offset >= 0, "Cannot seek after the end of the stream");
		position = getSize() - offset;
		break;

	}

	return position;

}

u64 ByteReader::tell() const {
	return position;
}



u64 ByteReader::getSize() const {
	return memory.size();
}

bool ByteReader::isOpen() const {
	return memory.size();
}
