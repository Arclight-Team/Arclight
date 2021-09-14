#include "byteoutputstream.h"




u64 ByteOutputStream::write(const void* src, u64 size) {
	return ByteStreamImplRW::write(src, size);
}



u64 ByteOutputStream::seek(i64 offset, SeekMode mode) {
	return ByteStreamImplRW::seek(offset, mode);
}



u64 ByteOutputStream::getPosition() const {
	return ByteStreamImplRW::getPosition();
}



u64 ByteOutputStream::getSize() const {
	return ByteStreamImplRW::getSize();
}



bool ByteOutputStream::isOpen() const {
	return ByteStreamImplRW::isOpen();
}