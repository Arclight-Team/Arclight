/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 syncbuffer.cpp
 */

#include "syncbuffer.hpp"

#include <algorithm>



SyncBuffer::SyncBuffer() {
	finishUpdate();
}



bool SyncBuffer::hasChanged() const noexcept {
	return updateStart < updateEnd;
}



SizeT SyncBuffer::size() const noexcept {
	return buffer.size();
}



void SyncBuffer::resize(SizeT newSize) {
	buffer.resize(newSize);
}



void SyncBuffer::clear() {
	buffer.clear();
}



const u8* SyncBuffer::data() const noexcept{
	return buffer.data();
}



const u8& SyncBuffer::operator[](SizeT offset) const {
	return buffer[offset];
}



const u8* SyncBuffer::updateStartData() const {
	return buffer.data() + updateStart;
}



SizeT SyncBuffer::getUpdateStart() const noexcept {
	return updateStart;
}



SizeT SyncBuffer::getUpdateEnd() const noexcept {
	return updateEnd;
}



SizeT SyncBuffer::getUpdateSize() const noexcept {
	return updateEnd - updateStart;
}



void SyncBuffer::write(SizeT offset, const std::span<const u8>& source) {
	std::copy_n(source.begin(), source.size(), buffer.data() + offset);
}



void SyncBuffer::updateBounds(SizeT offset, SizeT size) noexcept {

	if (offset < updateStart) {
		updateStart = offset;
	}

	if (offset + size > updateEnd) {
		updateEnd = offset + size;
	}

}



void SyncBuffer::finishUpdate() noexcept {

	updateEnd = 0;
	updateStart = -1;

}
