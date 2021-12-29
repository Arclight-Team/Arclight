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



SizeT FileInputStream::seek(i64 offset, SeekMode mode) {
	return FileStreamImpl::seek(offset, mode);
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