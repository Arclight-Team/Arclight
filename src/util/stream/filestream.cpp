#include "filestream.h"
#include "util/file.h"


FileStream::FileStream(File& file) : FileStreamImpl(file) {}


u64 FileStream::read(void* dest, u64 size) {
	return FileStreamImpl::read(dest, size);
}



u64 FileStream::write(const void* src, u64 size) {
	return FileStreamImpl::write(src, size);
}



u64 FileStream::seek(i64 offset, SeekMode mode) {
	return FileStreamImpl::seek(offset, mode);
}



u64 FileStream::getPosition() const {
	return FileStreamImpl::getPosition();
}



u64 FileStream::getSize() const {
	return FileStreamImpl::getSize();
}



bool FileStream::isOpen() const {
	return FileStreamImpl::isOpen();
}