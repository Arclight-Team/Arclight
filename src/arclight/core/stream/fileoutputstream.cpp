/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 fileoutputstream.cpp
 */

#include "fileoutputstream.hpp"
#include "filesystem/file.hpp"


FileOutputStream::FileOutputStream(File& file) : FileStreamImpl(file) {}



SizeT FileOutputStream::write(const void* src, SizeT size) {
	return FileStreamImpl::write(src, size);
}



void FileOutputStream::seek(i64 offset) {
	FileStreamImpl::seek(offset);
}



void FileOutputStream::seekTo(u64 offset) {
	FileStreamImpl::seekTo(offset);
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