#include "byteoutputstream.h"




SizeT ByteOutputStream::write(const void* src, SizeT size) {
	return ByteStreamImplRW::write(src, size);
}



SizeT ByteOutputStream::seek(i64 offset, SeekMode mode) {
	return ByteStreamImplRW::seek(offset, mode);
}



SizeT ByteOutputStream::getPosition() const {
	return ByteStreamImplRW::getPosition();
}



SizeT ByteOutputStream::getSize() const {
	return ByteStreamImplRW::getSize();
}



bool ByteOutputStream::isOpen() const {
	return ByteStreamImplRW::isOpen();
}


bool ByteOutputStream::reachedEnd() const {
	return ByteStreamImplRW::reachedEnd();
}