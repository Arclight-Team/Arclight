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
}



void SpriteTypeBuffer::addType(u32 typeID, const SpriteType& type, u32 ctID, u32 texID) {

	typeMap.try_emplace(typeID, static_cast<u32>(typeMap.size()));
	buffer.resize(buffer.size() + typeDataSize);
	setTypeData(typeID, type, ctID, texID);

}



void SpriteTypeBuffer::setTypeData(u32 typeID, const SpriteType& type, u32 ctID, u32 texID) {

	arc_assert(typeMap.contains(typeID), "Illegal partial update of sprite type buffer");

	SizeT offset = typeMap[typeID] * typeDataSize;

	buffer.write(offset + 0, type.origin);
	buffer.write(offset + 8, type.outline.uvBase);
	buffer.write(offset + 16, type.outline.uvScale);
	buffer.write(offset + 24, ctID);
	buffer.write(offset + 28, texID);

}



void SpriteTypeBuffer::clear() {

	typeMap.clear();
	buffer.clear();

	data = std::make_shared<SpriteTypeBufferData>();
	buffer.finishUpdate();

}



u32 SpriteTypeBuffer::getTypeIndex(u32 typeID) const {
	return typeMap.find(typeID)->second;
}



void SpriteTypeBuffer::update() {

	if (!buffer.hasChanged()) {
		return;
	}

	GLE::ShaderStorageBuffer& ssbo = data->typeSSBO;

	ssbo.bind();

	if (ssbo.getSize() < buffer.size()) {

		ssbo.allocate(Math::alignUp(buffer.size(), 2048), GLE::BufferAccess::DynamicDraw);
		ssbo.update(0, buffer.size(), buffer.data());

	} else {

		ssbo.update(buffer.getUpdateStart(), buffer.getUpdateSize(), buffer.updateStartData());

	}

	buffer.finishUpdate();

}



void SpriteTypeBuffer::bind() {

	GLE::ShaderStorageBuffer& ssbo = data->typeSSBO;
	ssbo.bindRange(0, 0, buffer.size());

}