/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 filestream.cpp
 */

#include "filestream.hpp"
#include "filesystem/file.hpp"


FileStream::FileStream(File& file) : FileStreamImpl(file) {}


SizeT FileStream::read(void* dest, SizeT size) {
	return FileStreamImpl::read(dest, size);
}



SizeT FileStream::write(const void* src, SizeT size) {
	return FileStreamImpl::write(src, size);
}



void FileStream::seek(i64 offset) {
	return FileStreamImpl::seek(offset);
}



void FileStream::seekTo(u64 offset) {
	return FileStreamImpl::seekTo(offset);
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