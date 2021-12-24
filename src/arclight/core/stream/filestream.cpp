#include "filestream.hpp"
#include "filesystem/file.hpp"


FileStream::FileStream(File& file) : FileStreamImpl(file) {}


SizeT FileStream::read(void* dest, SizeT size) {
	return FileStreamImpl::read(dest, size);
}



SizeT FileStream::write(const void* src, SizeT size) {
	return FileStreamImpl::write(src, size);
}



SizeT FileStream::seek(i64 offset, SeekMode mode) {
	return FileStreamImpl::seek(offset, mode);
}



SizeT FileStream::getPosition() const {
	return FileStreamImpl::getPosition();
}



SizeT FileStream::getSize() const {
	return FileStreamImpl::getSize();
}



bool FileStream::isOpen() const {
	return FileStreamImpl::isOpen();
}


bool FileStream::reachedEnd() const {
	return FileStreamImpl::reachedEnd();
}