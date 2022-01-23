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



void ByteInputStream::seek(i64 offset) {
	ByteStreamImplR::seek(offset);
}




void ByteInputStream::seekTo(u64 offset) {
	ByteStreamImplR::seekTo(offset);
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