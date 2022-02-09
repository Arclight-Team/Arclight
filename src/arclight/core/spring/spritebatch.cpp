/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritebatch.cpp
 */

#include "spritebatch.hpp"
#include "util/assert.hpp"
#include "render/gle/gle.hpp"
#include "render/utility/vertexhelper.hpp"




struct SpriteBatchData {

	SpriteBatchData() {

		constexpr float squareOutline[] = {
			-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
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
		vao.setAttribute(2, 4, GLE::AttributeType::Float, 32, 16);
		vao.enableAttribute(1);
		vao.enableAttribute(2);
		vao.setDivisor(1, 1);
		vao.setDivisor(2, 1);

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



SpriteBatch::SpriteBatch() {

	data = std::make_shared<SpriteBatchData>();
	resetBounds();

}



void SpriteBatch::createSprite(u64 id, const Vec2f& translation, const Vec2f& scale, const Mat2f& rsTransform) {

	SizeT offset = vertexData.size();

	offsets[id] = offset;
	ids[offset] = id;
	vertexData.resize(offset + dataSize);

	setSpriteRSTransform(id, rsTransform);
	setSpriteTranslation(id, translation);
	setSpriteScale(id, scale);

}



void SpriteBatch::setSpriteTranslation(u64 id, const Vec2f& translation) {

	arc_assert(offsets.contains(id), "Illegal incomplete sprite batch update");

	u8 flatVec[8];
	vectorDecay(translation, flatVec);

	SizeT offset = offsets[id] + translationOffset;
	std::copy_n(flatVec, 8, &vertexData[offset]);
	updateBounds(offset, 8);

}



void SpriteBatch::setSpriteScale(u64 id, const Vec2f& scale) {

	arc_assert(offsets.contains(id), "Illegal incomplete sprite batch update");

	u8 flatVec[8];
	vectorDecay(scale, flatVec);

	SizeT offset = offsets[id] + scaleOffset;
	std::copy_n(flatVec, 8, &vertexData[offset]);
	updateBounds(offset, 8);

}



void SpriteBatch::setSpriteRSTransform(u64 id, const Mat2f& rsTransform) {

	arc_assert(offsets.contains(id), "Illegal incomplete sprite batch update");

	u8 flatMat[16];
	matrixDecay(rsTransform, flatMat);

	SizeT offset = offsets[id] + transformOffset;
	std::copy_n(flatMat, 16, &vertexData[offset]);
	updateBounds(offset, 16);

}



void SpriteBatch::purgeSprite(u64 id) {

	auto it = offsets.find(id);

	if (it != offsets.end()) {

		SizeT offset = it->second;
		SizeT lastOffset = vertexData.size() - dataSize;
		u64 lastSprite = ids[lastOffset];

		std::copy_n(&vertexData[lastOffset], dataSize, &vertexData[offset]);

		ids.erase(lastOffset);
		offsets.erase(lastSprite);

	}

}



void SpriteBatch::synchronize() {

	if (updateStart >= updateEnd) {
		return;
	}

	GLE::VertexBuffer& matrixVbo = data->matrixVbo;

	matrixVbo.bind();

	if (matrixVbo.getSize() < vertexData.size()) {

		matrixVbo.allocate(Math::alignUp(vertexData.size(), 2048), GLE::BufferAccess::DynamicDraw);
		matrixVbo.update(0, vertexData.size(), vertexData.data());

	} else {

		matrixVbo.update(updateStart, updateEnd - updateStart, vertexData.data() + updateStart);

	}

	resetBounds();

}



void SpriteBatch::render() {

	data->vao.bind();
	GLE::renderInstanced(GLE::PrimType::Triangle, getBatchSize(), 6);

}



SizeT SpriteBatch::getBatchSize() const {
	return offsets.size();
}



void SpriteBatch::updateBounds(SizeT offset, SizeT size) {

	if (offset < updateStart) {
		updateStart = offset;
	}

	if (offset + size > updateEnd) {
		updateEnd = offset + size;
	}

}



void SpriteBatch::resetBounds() noexcept {

	updateEnd = 0;
	updateStart = -1;

}



void SpriteBatch::vectorDecay(const Vec2f& vector, const std::span<u8>& dest) {

	u32 x = 0;

	for (u32 i = 0; i < 2; i++, x += 4) {
		std::copy_n(Bits::toByteArray(&vector[i]), 4, &dest[x]);
	}

}



void SpriteBatch::matrixDecay(const Mat2f& matrix, const std::span<u8>& dest) {

	u32 x = 0;

	for (u32 i = 0; i < 2; i++) {

		for (u32 j = 0; j < 2; j++, x += 4) {
			std::copy_n(Bits::toByteArray(&matrix[i][j]), 4, &dest[x]);
		}

	}

}