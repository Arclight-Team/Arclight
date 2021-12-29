/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 byteinputstream.cpp
 */

#include "byteinputstream.hpp"



SizeT ByteInputStream::read(void* dest, SizeT size) {
	return ByteStreamImplR::read(dest, size);
}



SizeT ByteInputStream::seek(i64 offset, SeekMode mode) {
	return ByteStreamImplR::seek(offset, mode);
}



SizeT ByteInputStream::getPosition() const {
	return ByteStreamImplR::getPosition();
}



SizeT ByteInputStream::getSize() const {
	return ByteStreamImplR::getSize();
}



bool ByteInputStream::isOpen() const {
	return ByteStreamImplR::isOpen();
}


bool ByteInputStream::reachedEnd() const {
	return ByteStreamImplR::reachedEnd();
}