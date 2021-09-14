#include "byteinputstream.h"



u64 ByteInputStream::read(void* dest, u64 size) {
	return ByteStreamImplR::read(dest, size);
}



u64 ByteInputStream::seek(i64 offset, SeekMode mode) {
	return ByteStreamImplR::seek(offset, mode);
}



u64 ByteInputStream::getPosition() const {
	return ByteStreamImplR::getPosition();
}



u64 ByteInputStream::getSize() const {
	return ByteStreamImplR::getSize();
}



bool ByteInputStream::isOpen() const {
	return ByteStreamImplR::isOpen();
}