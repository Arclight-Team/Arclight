/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritebatch.hpp
 */

#pragma once

#include "math/matrix.hpp"
#include "memory/alignedallocator.hpp"
#include "types.hpp"

#include <vector>
#include <span>



class SpriteBatchData;

class SpriteBatch {

public:

	SpriteBatch();

	void createSprite(u64 id, const Mat3f& affine);
	void setSpriteTranslation(u64 id, const Vec2f& position);
	void setSpriteMatrix(u64 id, const Mat3f& affine);
	void purgeSprite(u64 id);
	void synchronize();
	void render();

	SizeT getBatchSize() const;

private:

	constexpr static u32 dataSize = 32;

	void updateBounds(SizeT offset, SizeT size);
	void resetBounds() noexcept;

	static void vectorDecay(const Vec2f& vector, const std::span<u8>& dest);
	static void matrixDecay(const Mat3f& matrix, const std::span<u8>& dest);


	SizeT updateStart, updateEnd;

	std::shared_ptr<SpriteBatchData> data;

	std::unordered_map<u64, SizeT> offsets;
	std::unordered_map<SizeT, u64> ids;
	std::vector<u8, AlignedAllocator<u8, 16>> vertexData;

};