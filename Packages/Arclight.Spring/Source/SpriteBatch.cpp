/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SpriteBatch.cpp
 */

#include "Spring/SpriteBatch.hpp"
#include "Spring/VertexHelper.hpp"
#include "Common/Assert.hpp"
#include "Render/GLE/GLE.hpp"




struct SpriteBatchData {

	SpriteBatchData() {

		constexpr float squareOutline[] = {
			0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
		};

		vao.create();
		vao.bind();

		outlineVbo.create();
		outlineVbo.bind();
		outlineVbo.allocate(sizeof(squareOutline), squareOutline, GLE::BufferAccess::StaticDraw);

		vao.setAttribute(0, 2, GLE::AttributeType::Float, 0, 0);
		vao.enableAttribute(0);

		matrixVbo.create();
		matrixVbo.bind();

		vao.setAttribute(1, 4, GLE::AttributeType::Float, 32, 0);
		vao.setAttribute(2, 2, GLE::AttributeType::Float, 32, 16);
		vao.setAttribute(3, 1, GLE::AttributeType::UInt, 32, 24, GLE::AttributeClass::Int);
		vao.enableAttribute(1);
		vao.enableAttribute(2);
		vao.enableAttribute(3);
		vao.setDivisor(1, 1);
		vao.setDivisor(2, 1);
		vao.setDivisor(3, 1);

	}

	~SpriteBatchData() {

		if (!vao.isCreated()) {
			return;
		}

		vao.destroy();
		outlineVbo.destroy();
		matrixVbo.destroy();

	}

	GLE::VertexArray vao;
	GLE::VertexBuffer matrixVbo;
	GLE::VertexBuffer outlineVbo;

};



void SpriteBatch::create() {

	if (!data) {
		data = std::make_shared<SpriteBatchData>();
	}

}



void SpriteBatch::createSprite(u64 id, u32 typeIndex, const Vec2f& translation, const Mat2f& transform) {

	SizeT offset = buffer.size();

	offsets[id] = offset;
	ids[offset] = id;
	buffer.resize(offset + dataSize);

	setSpriteTransform(id, transform);
	setSpriteTranslation(id, translation);
	setSpriteTypeIndex(id, typeIndex);

}



void SpriteBatch::setSpriteTranslation(u64 id, const Vec2f& translation) {

	arc_assert(offsets.contains(id), "Illegal incomplete sprite batch update");

	SizeT offset = offsets[id] + translationOffset;
	buffer.write(offset, translation);

}



void SpriteBatch::setSpriteTransform(u64 id, const Mat2f& transform) {

	arc_assert(offsets.contains(id), "Illegal incomplete sprite batch update");

	SizeT offset = offsets[id] + transformOffset;
	buffer.write(offset, transform);

}



void SpriteBatch::setSpriteTypeIndex(u64 id, u32 typeIndex) {

	arc_assert(offsets.contains(id), "Illegal incomplete sprite batch update");

	SizeT offset = offsets[id] + typeIndexOffset;
	buffer.write(offset, typeIndex);

}



void SpriteBatch::purgeSprite(u64 id) {

	auto it = offsets.find(id);

	if (it != offsets.end()) {

		SizeT offset = it->second;
		SizeT lastOffset = buffer.size() - dataSize;

		if (offset != lastOffset) {

			u64 lastSprite = ids[lastOffset];

			buffer.write(offset, {&buffer[lastOffset], dataSize});

			ids[offset] = lastSprite;
			offsets[lastSprite] = offset;

		}

		ids.erase(lastOffset);
		offsets.erase(id);

		buffer.resize(lastOffset);

	}

}



void SpriteBatch::synchronize() {

	if (!buffer.hasChanged()) {
		return;
	}

	GLE::VertexBuffer& matrixVbo = data->matrixVbo;
	matrixVbo.bind();

	if (matrixVbo.getSize() < buffer.size()) {

		matrixVbo.allocate(Bits::ceilPowerOf2(buffer.size(), 2048), GLE::BufferAccess::DynamicDraw);
		matrixVbo.update(0, buffer.size(), buffer.data());

	} else {

		matrixVbo.update(buffer.getUpdateStart(), buffer.getUpdateSize(), buffer.updateStartData());

	}

	buffer.finishUpdate();

}



void SpriteBatch::render() {

	data->vao.bind();
	GLE::renderInstanced(GLE::PrimType::Triangle, getBatchSize(), 6);

}



SizeT SpriteBatch::getBatchSize() const {
	return offsets.size();
}