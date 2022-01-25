/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 fileinputstream.cpp
 */

#include "fileinputstream.hpp"
#include "filesystem/file.hpp"


FileInputStream::FileInputStream(File& file) : FileStreamImpl(file) {}


SizeT FileInputStream::read(void* dest, SizeT size) {
	return FileStreamImpl::read(dest, size);
}



void FileInputStream::seek(i64 offset) {
	FileStreamImpl::seek(offset);
}



void FileInputStream::seekTo(u64 offset) {
	FileStreamImpl::seekTo(offset);
}



SizeT FileInputStream::getPosition() const {
	return FileStreamImpl::getPosition();
}



SizeT FileInputStream::getSize() const {
	return FileStreamImpl::getSize();
}



bool FileInputStream::isOpen() const {
	return FileStreamImpl::isOpen();
}


bool FileInputStream::reachedEnd() const {
	return FileStreamImpl::reachedEnd();
}