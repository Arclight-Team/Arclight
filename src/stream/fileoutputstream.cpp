#include "fileoutputstream.h"
#include "util/file.h"


FileOutputStream::FileOutputStream(File& file) : FileStreamImpl(file) {}



SizeT FileOutputStream::write(const void* src, SizeT size) {
	return FileStreamImpl::write(src, size);
}



SizeT FileOutputStream::seek(i64 offset, StreamBase::SeekMode mode) {
	return FileStreamImpl::seek(offset, mode);
}



SizeT FileOutputStream::getPosition() const {
	return FileStreamImpl::getPosition();
}



SizeT FileOutputStream::getSize() const {
	return FileStreamImpl::getSize();
}



bool FileOutputStream::isOpen() const {
	return FileStreamImpl::isOpen();
}


bool FileOutputStream::reachedEnd() const {
	return FileStreamImpl::reachedEnd();
}