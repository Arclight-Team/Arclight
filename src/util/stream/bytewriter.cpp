#include "bytewriter.h"
#include "util/math.h"
#include "util/assert.h"


ByteWriter::ByteWriter(u8* data, u64 size) : memory(static_cast<std::byte*>(static_cast<void*>(data)), size), position(0) {}


u64 ByteWriter::read(void* dest, u64 size) {

	arc_force_assert("Cannot read from write-only stream");
	return 0;

}

u64 ByteWriter::write(const void* src, u64 size) {

	arc_assert(src != nullptr, "Source is null");
	arc_assert(position + size <= getSize(), "Cannot write past the end of the stream");

	size = Math::min(size, getSize() - position);
	std::memcpy(memory.data() + position, src, size);

	position += size;

	return size;

}



u64 ByteWriter::seek(i64 offset, SeekMode mode) {

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

u64 ByteWriter::tell() const {
	return position;
}



u64 ByteWriter::getSize() const {
	return memory.size();
}

bool ByteWriter::isOpen() const {
	return memory.size();
}
