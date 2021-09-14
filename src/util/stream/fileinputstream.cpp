#include "fileinputstream.h"
#include "util/file.h"


FileInputStream::FileInputStream(File& file) : FileStreamImpl(file) {}


u64 FileInputStream::read(void* dest, u64 size) {
	return FileStreamImpl::read(dest, size);
}



u64 FileInputStream::seek(i64 offset, SeekMode mode) {
	return FileStreamImpl::seek(offset, mode);
}



u64 FileInputStream::getPosition() const {
	return FileStreamImpl::getPosition();
}



u64 FileInputStream::getSize() const {
	return FileStreamImpl::getSize();
}



bool FileInputStream::isOpen() const {
	return FileStreamImpl::isOpen();
}