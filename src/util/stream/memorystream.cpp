#include "memorystream.h"
#include "util/math.h"


MemoryStream::MemoryStream(const u8* data, u64 size) : memory(data, data + size), position(0) {}

MemoryStream::MemoryStream(const std::vector<u8>& data) : memory(data), position(0) {}



u64 MemoryStream::read(void* dest, u64 size) {

	arc_assert(dest != nullptr, "Destination is null");
	arc_assert(position + size <= getSize(), "Cannot read after the end of the stream");

	size = Math::min(size, getSize() - position);

	std::memcpy(dest, memory.data() + position, size);
	
	position += size;

	return size;

}

u64 MemoryStream::write(const void* src, u64 size) {

	arc_assert(src != nullptr, "Source is null");
	arc_assert(position + size <= getSize(), "Cannot write after the end of the stream");

	size = Math::min(size, getSize() - position);

	std::memcpy(memory.data() + position, src, size);

	position += size;

	return size;

}



u64 MemoryStream::seek(i64 offset, SeekMode mode) {

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

u64 MemoryStream::tell() const {
	return position;
}



u64 MemoryStream::getSize() const {
	return memory.size();
}

bool MemoryStream::isOpen() const {
	return !memory.empty();
}
