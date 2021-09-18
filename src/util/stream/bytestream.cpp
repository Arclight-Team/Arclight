#include "bytestream.h"



SizeT ByteStream::read(void* dest, SizeT size) {
	return ByteStreamImplRW::read(dest, size);
}



SizeT ByteStream::write(const void* src, SizeT size) {
	return ByteStreamImplRW::write(src, size);
}



SizeT ByteStream::seek(i64 offset, SeekMode mode) {
	return ByteStreamImplRW::seek(offset, mode);
}



SizeT ByteStream::getPosition() const {
	return ByteStreamImplRW::getPosition();
}



SizeT ByteStream::getSize() const {
	return ByteStreamImplRW::getSize();
}



bool ByteStream::isOpen() const {
	return ByteStreamImplRW::isOpen();
}


bool ByteStream::reachedEnd() const {
	return ByteStreamImplRW::reachedEnd();
}