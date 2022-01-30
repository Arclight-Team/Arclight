/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritearray.hpp
 */

#pragma once

#include "sprite.hpp"
#include "stdext/optionalref.hpp"
#include "util/uuid.hpp"



class SpriteArray {

public:

	SpriteArray();

	Sprite& create(u64 id);
	bool contains(u64 id) const;
	void destroy(u64 id);
	void clear();

	Sprite& get(u64 id) noexcept;
	Sprite& getOrThrow(u64 id);
	OptionalRef<Sprite> getOrNull(u64 id) noexcept;

	const Sprite& get(u64 id) const noexcept;
	const Sprite& getOrThrow(u64 id) const;
	OptionalRef<const Sprite> getOrNull(u64 id) const noexcept;

private:

	constexpr static u32 spriteDomainSize = 512;
	constexpr static u32 invalidStorage = -1;

	struct Storage {

		static_assert(std::is_nothrow_constructible_v<Sprite>, "Sprite must be nothrow-constructible");

		constexpr Storage() noexcept : next(invalidStorage) {}

		Sprite sprite;
		u32 next;

	};

	const Sprite& spriteByIndex(u32 idx) const noexcept;

	Storage& storageByIndex(u32 idx) noexcept;
	const Storage& storageByIndex(u32 idx) const noexcept;


	std::unordered_map<u64, u32> spriteIDMapping;
	std::vector<std::vector<Storage>> sprites;

	u32 firstActive;
	u32 firstFree;

};