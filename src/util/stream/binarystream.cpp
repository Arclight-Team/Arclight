#include "binarystream.h"


BinaryStream::BinaryStream(Stream& stream) : stream(stream), bigEndian(false) {}
BinaryStream::BinaryStream(Stream& stream, bool bigEndian) : stream(stream), bigEndian(bigEndian) {}



u64 BinaryStream::read(void* dest, u64 size) {

	arc_assert(dest != nullptr, "Destination is null");

	return stream.read(dest, size);

}

u64 BinaryStream::write(const void* src, u64 size) {

	arc_assert(src != nullptr, "Source is null");

	return stream.write(src, size);

}



void BinaryStream::seek(i64 offset, SeekMode mode) {
	stream.seek(offset, mode);
}

u64 BinaryStream::tell() const {
	return stream.tell();
}



u64 BinaryStream::getSize() const {
	return stream.getSize();
}

bool BinaryStream::isOpen() const {
	return stream.isOpen();
}