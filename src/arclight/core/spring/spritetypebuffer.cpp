/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritetypebuffer.cpp
 */

#include "spritetypebuffer.hpp"
#include "spritetype.hpp"
#include "util/assert.hpp"
#include "util/bits.hpp"
#include "render/gle/gle.hpp"



struct SpriteTypeBufferData {

	SpriteTypeBufferData() {

		typeSSBO.create();
		typeSSBO.bind();
		typeSSBO.allocate(2048, GLE::BufferAccess::DynamicDraw);
		typeSSBO.bindRange(0, 0, typeSSBO.getSize());

	}

	~SpriteTypeBufferData() {
		typeSSBO.destroy();
	}

	GLE::ShaderStorageBuffer typeSSBO;

};



SpriteTypeBuffer::SpriteTypeBuffer() {

	data = std::make_shared<SpriteTypeBufferData>();
	resetBounds();

}



void SpriteTypeBuffer::addType(u32 typeID, const SpriteType& type) {

	typeMap.try_emplace(typeID, static_cast<u32>(typeMap.size()));
	buffer.resize(buffer.size() + typeDataSize);
	setTypeData(typeID, type);

}



void SpriteTypeBuffer::setTypeData(u32 typeID, const SpriteType& type) {

	arc_assert(typeMap.contains(typeID), "Illegal partial update of sprite type buffer");

	SizeT offset = typeMap[typeID] * typeDataSize;
	std::copy_n(Bits::toByteArray(&type.origin.x), 4, &buffer[offset]);
	std::copy_n(Bits::toByteArray(&type.origin.y), 4, &buffer[offset + 4]);

	updateBounds(offset, typeDataSize);

}



void SpriteTypeBuffer::clear() {

	typeMap.clear();
	buffer.clear();

	data = std::make_shared<SpriteTypeBufferData>();
	resetBounds();

}



u32 SpriteTypeBuffer::getTypeIndex(u32 typeID) const {
	return typeMap.find(typeID)->second;
}



void SpriteTypeBuffer::update() {

	if (updateStart >= updateEnd) {
		return;
	}

	GLE::ShaderStorageBuffer& ssbo = data->typeSSBO;

	ssbo.bind();

	if (ssbo.getSize() < buffer.size()) {

		ssbo.allocate(Math::alignUp(buffer.size(), 2048), GLE::BufferAccess::DynamicDraw);
		ssbo.update(0, buffer.size(), buffer.data());

	} else {

		ssbo.update(updateStart, updateEnd - updateStart, buffer.data() + updateStart);

	}

	resetBounds();

}



void SpriteTypeBuffer::bind() {

	GLE::ShaderStorageBuffer& ssbo = data->typeSSBO;
	ssbo.bind();
	ssbo.bindRange(0, 0, buffer.size());

}



void SpriteTypeBuffer::updateBounds(SizeT offset, SizeT size) {

	if (offset < updateStart) {
		updateStart = offset;
	}

	if (offset + size > updateEnd) {
		updateEnd = offset + size;
	}

}



void SpriteTypeBuffer::resetBounds() {

	updateStart = 0;
	updateEnd = -1;

}