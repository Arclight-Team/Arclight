#include "fileoutputstream.h"
#include "util/file.h"


FileOutputStream::FileOutputStream(File& file) : FileStreamImpl(file) {}



u64 FileOutputStream::write(const void* src, u64 size) {
	return FileStreamImpl::write(src, size);
}



u64 FileOutputStream::seek(i64 offset, StreamBase::SeekMode mode) {
	return FileStreamImpl::seek(offset, mode);
}



u64 FileOutputStream::getPosition() const {
	return FileStreamImpl::getPosition();
}



u64 FileOutputStream::getSize() const {
	return FileStreamImpl::getSize();
}



bool FileOutputStream::isOpen() const {
	return FileStreamImpl::isOpen();
}