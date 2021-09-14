#include "bytestream.h"



u64 ByteStream::read(void* dest, u64 size) {
	return ByteStreamImplRW::read(dest, size);
}



u64 ByteStream::write(const void* src, u64 size) {
	return ByteStreamImplRW::write(src, size);
}



u64 ByteStream::seek(i64 offset, SeekMode mode) {
	return ByteStreamImplRW::seek(offset, mode);
}



u64 ByteStream::getPosition() const {
	return ByteStreamImplRW::getPosition();
}



u64 ByteStream::getSize() const {
	return ByteStreamImplRW::getSize();
}



bool ByteStream::isOpen() const {
	return ByteStreamImplRW::isOpen();
}